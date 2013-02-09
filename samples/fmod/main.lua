MOAISim.openWindow ( "test", 320, 480 )

viewport = MOAIViewport.new ()
viewport:setSize ( 320, 480 )
viewport:setScale ( 320, -480 )

layer = MOAILayer2D.new ()
layer:setViewport ( viewport )
MOAISim.pushRenderPass ( layer )

gfxQuad = MOAIGfxQuad2D.new ()
gfxQuad:setTexture ( "moai.png" )
gfxQuad:setRect ( -64, -64, 64, 64 )
gfxQuad:setUVRect ( 0, 0, 1, 1 )

prop = MOAIProp2D.new ()
prop:setDeck ( gfxQuad )
layer:insertProp ( prop )

prop:moveRot ( 360, 1.5 )

-------------------------------------

MOAIFmodEx.init ()

local file = "track.ogg"
local stream = false
local async = false

sound = MOAIFmodExSound.new ()
sound:load ( file, stream, async )

channel = MOAIFmodExChannel.new ()
channel:playWithLoopPoint ( sound, 20.611338 )

MOAIInputMgr.device.keyboard:setCallback ( onKeyboardEvent )