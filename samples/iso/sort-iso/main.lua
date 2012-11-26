----------------------------------------------------------------
-- Copyright (c) 2010-2011 Zipline Games, Inc. 
-- All Rights Reserved. 
-- http://getmoai.com
----------------------------------------------------------------

MOAISim.openWindow ( "test", 1024, 640 )

viewport = MOAIViewport.new ()
viewport:setSize ( 1024, 640 )
viewport:setScale ( 1024, 640 )

layer = MOAILayer.new ()
layer:setViewport ( viewport )
layer:setSortMode ( MOAILayer.SORT_ISO )
MOAISim.pushRenderPass ( layer )

camera = MOAICamera.new ()
camera:setOrtho ( true )
camera:setNearPlane ( 10000 )
camera:setFarPlane ( -10000 )
camera:setRot ( 54.736, 0, 135 )
layer:setCamera ( camera )

texture = MOAITexture.new ()
texture:load ( 'moai.png' )

function makeBoxMesh ( xMin, yMin, zMin, xMax, yMax, zMax, texture )
	
	local function pushPoint ( points, x, y, z )
	
		local point = {}
		point.x = x
		point.y = y
		point.z = z
		
		table.insert ( points, point )
	end

	local function writeTri ( vbo, p1, p2, p3, uv1, uv2, uv3 )
		
		vbo:writeFloat ( p1.x, p1.y, p1.z )
		vbo:writeFloat ( uv1.x, uv1.y )
		vbo:writeColor32 ( 1, 1, 1 )
		
		vbo:writeFloat ( p2.x, p2.y, p2.z )
		vbo:writeFloat ( uv2.x, uv2.y )
		vbo:writeColor32 ( 1, 1, 1 )

		vbo:writeFloat ( p3.x, p3.y, p3.z )
		vbo:writeFloat ( uv3.x, uv3.y  )
		vbo:writeColor32 ( 1, 1, 1 )
	end
	
	local function writeFace ( vbo, p1, p2, p3, p4, uv1, uv2, uv3, uv4 )

		writeTri ( vbo, p1, p2, p4, uv1, uv2, uv4 )
		writeTri ( vbo, p2, p3, p4, uv2, uv3, uv4 )
	end
	
	local p = {}
	
	pushPoint ( p, xMin, yMax, zMax ) -- p1
	pushPoint ( p, xMin, yMin, zMax ) -- p2
	pushPoint ( p, xMax, yMin, zMax ) -- p3
	pushPoint ( p, xMax, yMax, zMax ) -- p4
	
	pushPoint ( p, xMin, yMax, zMin ) -- p5
	pushPoint ( p, xMin, yMin, zMin  ) -- p6
	pushPoint ( p, xMax, yMin, zMin  ) -- p7
	pushPoint ( p, xMax, yMax, zMin  ) -- p8

	local uv = {}
	
	pushPoint ( uv, 0, 0, 0 )
	pushPoint ( uv, 0, 1, 0 )
	pushPoint ( uv, 1, 1, 0 )
	pushPoint ( uv, 1, 0, 0 )
	
	local vertexFormat = MOAIVertexFormat.new ()
	vertexFormat:declareCoord ( 1, MOAIVertexFormat.GL_FLOAT, 3 )
	vertexFormat:declareUV ( 2, MOAIVertexFormat.GL_FLOAT, 2 )
	vertexFormat:declareColor ( 3, MOAIVertexFormat.GL_UNSIGNED_BYTE )

	local vbo = MOAIVertexBuffer.new ()
	vbo:setFormat ( vertexFormat )
	vbo:reserveVerts ( 36 )
	
	writeFace ( vbo, p [ 1 ], p [ 2 ], p [ 3 ], p [ 4 ], uv [ 1 ], uv [ 2 ], uv [ 3 ], uv [ 4 ])
	writeFace ( vbo, p [ 4 ], p [ 3 ], p [ 7 ], p [ 8 ], uv [ 1 ], uv [ 2 ], uv [ 3 ], uv [ 4 ])
	writeFace ( vbo, p [ 8 ], p [ 7 ], p [ 6 ], p [ 5 ], uv [ 1 ], uv [ 2 ], uv [ 3 ], uv [ 4 ])
	writeFace ( vbo, p [ 5 ], p [ 6 ], p [ 2 ], p [ 1 ], uv [ 1 ], uv [ 2 ], uv [ 3 ], uv [ 4 ])
	writeFace ( vbo, p [ 5 ], p [ 1 ], p [ 4 ], p [ 8 ], uv [ 1 ], uv [ 2 ], uv [ 3 ], uv [ 4 ])
	writeFace ( vbo, p [ 2 ], p [ 6 ], p [ 7 ], p [ 3 ], uv [ 1 ], uv [ 2 ], uv [ 3 ], uv [ 4 ])

	vbo:bless ()

	local mesh = MOAIMesh.new ()
	mesh:setTexture ( texture )
	mesh:setVertexBuffer ( vbo )
	mesh:setPrimType ( MOAIMesh.GL_TRIANGLES )
	
	return mesh
end

function makeMeshProp ( layer, mesh )

	local prop = MOAIProp.new ()
	prop:setDeck ( mesh )
	prop:setShader ( MOAIShaderMgr.getShader ( MOAIShaderMgr.MESH_SHADER ))
	prop:setCullMode ( MOAIProp.CULL_BACK )
	layer:insertProp ( prop )
	
	return prop
end

function makeCube ( size, texture )
	size = size * 0.5
	return makeBoxMesh ( -size, -size, -size, size, size, size, texture )
end

function makeColumn ( base, height, texture )
	base = base * 0.5
	return makeBoxMesh ( -base, -base, 0, base, base, height, texture )
end

function makeBox ( width, depth, height, texture )
	width = width * 0.5
	depth = depth * 0.5
	return makeBoxMesh ( -width, -depth, 0, width, depth, height, texture )
end

local props = {}

props [ 1 ] = makeMeshProp ( layer, makeBox ( 96, 96, 128, texture ))
props [ 1 ]:setLoc ( 128, 0 )

props [ 2 ] = makeMeshProp ( layer, makeBox ( 224, 96, 128, texture ))
props [ 2 ]:setLoc ( 32, 128 )

props [ 3 ] = makeMeshProp ( layer, makeBox ( 96, 96, 128, texture ))
props [ 3 ]:setLoc ( 0, 0 )

function heroMain ()

	local keyboard = MOAIInputMgr.device.keyboard
	local heroID = 3
	local x = 0
	local y = 0
	
	while true do

		if keyboard:keyIsDown ( 'a' ) then
			y = y - 2
		end
		
		if keyboard:keyIsDown ( 'd' ) then
			y = y + 2
		end
		
		if keyboard:keyIsDown ( 'w' ) then
			x = x - 2
		end
		
		if keyboard:keyIsDown ( 's' ) then
			x = x + 2
		end
		
		if keyboard:keyDown ( ' ' ) then
			heroID = heroID + 1
			if #props < heroID then heroID = 1 end
			x, y = props [ heroID ]:getLoc ()
		end
		
		props [ heroID ]:setLoc ( x, y, 0 )
		
		coroutine.yield ()
	end
end

heroThread = MOAICoroutine.new ()
heroThread:run ( heroMain )