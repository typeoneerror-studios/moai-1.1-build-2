#ifndef HEADER_CURLMSG_H
#define HEADER_CURLMSG_H

#pragma __member_alignment __save
#pragma __nomember_alignment

/* $Id: curlmsg.h,v 1.8 2009-06-11 11:57:46 yangtse Exp $ */
/*                                                                          */
/* CURLMSG.H                                                                */
/*                                                                          */
/* SDL File Generated by VAX-11 Message V04-00 on 3-SEP-2008 13:33:54.09    */
/*                                                                          */
/* THESE VMS ERROR CODES ARE GENERATED BY TAKING APART THE CURL.H           */
/* FILE AND PUTTING ALL THE CURLE_* ENUM STUFF INTO THIS FILE,              */
/* CURLMSG.MSG.  AN .SDL FILE IS CREATED FROM THIS FILE WITH                */
/* MESSAGE/SDL.  THE .H FILE IS CREATED USING THE FREEWARE SDL TOOL         */
/* AGAINST THE .SDL FILE WITH SDL/ALPHA/LANG=CC COMMAND.                    */
/*                                                                          */
/* WITH THE EXCEPTION OF CURLE_OK, ALL OF THE MESSAGES ARE AT               */
/* THE ERROR SEVERITY LEVEL.  WITH THE EXCEPTION OF                         */
/* PEER_FAILED_VERIF, WHICH IS A SHORTENED FORM OF                          */
/* PEER_FAILED_VERIFICATION, THESE ARE THE SAME NAMES AS THE                */
/* CURLE_ ONES IN INCLUDE/CURL.H.  THE MESSAGE UTILITY MANUAL STATES        */
/* "THE COMBINED LENGTH OF THE PREFIX AND THE MESSAGE SYMBOL NAME CANNOT    */
/* EXCEED 31 CHARACTERS."  WITH A PREFIX OF FIVE THAT LEAVES US WITH 26     */
/* FOR THE MESSAGE NAME.                                                    */
/*                                                                          */
/* IF YOU UPDATE THIS FILE, UPDATE CURLMSG_VMS.H SO THAT THEY ARE IN SYNC   */
/*                                                                          */

#define CURL_FACILITY 3841
#define CURL_OK 251756553
#define CURL_UNSUPPORTED_PROTOCOL 251756562
#define CURL_FAILED_INIT 251756570
#define CURL_URL_MALFORMAT 251756578
#define CURL_OBSOLETE4 251756586
#define CURL_COULDNT_RESOLVE_PROXY 251756594
#define CURL_COULDNT_RESOLVE_HOST 251756602
#define CURL_COULDNT_CONNECT 251756610
#define CURL_FTP_WEIRD_SERVER_REPLY 251756618
#define CURL_FTP_ACCESS_DENIED 251756626
#define CURL_OBSOLETE10 251756634
#define CURL_FTP_WEIRD_PASS_REPLY 251756642
#define CURL_OBSOLETE12 251756650
#define CURL_FTP_WEIRD_PASV_REPLY 251756658
#define CURL_FTP_WEIRD_227_FORMAT 251756666
#define CURL_FTP_CANT_GET_HOST 251756674
#define CURL_OBSOLETE16 251756682
#define CURL_FTP_COULDNT_SET_TYPE 251756690
#define CURL_PARTIAL_FILE 251756698
#define CURL_FTP_COULDNT_RETR_FILE 251756706
#define CURL_OBSOLETE20 251756714
#define CURL_QUOTE_ERROR 251756722
#define CURL_HTTP_RETURNED_ERROR 251756730
#define CURL_WRITE_ERROR 251756738
#define CURL_OBSOLETE24 251756746
#define CURL_UPLOAD_FAILED 251756754
#define CURL_READ_ERROR 251756762
#define CURL_OUT_OF_MEMORY 251756770
#define CURL_OPERATION_TIMEOUTED 251756778
#define CURL_OBSOLETE29 251756786
#define CURL_FTP_PORT_FAILED 251756794
#define CURL_FTP_COULDNT_USE_REST 251756802
#define CURL_OBSOLETE32 251756810
#define CURL_RANGE_ERROR 251756818
#define CURL_HTTP_POST_ERROR 251756826
#define CURL_SSL_CONNECT_ERROR 251756834
#define CURL_BAD_DOWNLOAD_RESUME 251756842
#define CURL_FILE_COULDNT_READ_FILE 251756850
#define CURL_LDAP_CANNOT_BIND 251756858
#define CURL_LDAP_SEARCH_FAILED 251756866
#define CURL_OBSOLETE40 251756874
#define CURL_FUNCTION_NOT_FOUND 251756882
#define CURL_ABORTED_BY_CALLBACK 251756890
#define CURL_BAD_FUNCTION_ARGUMENT 251756898
#define CURL_OBSOLETE44 251756906
#define CURL_INTERFACE_FAILED 251756914
#define CURL_OBSOLETE46 251756922
#define CURL_TOO_MANY_REDIRECTS 251756930
#define CURL_UNKNOWN_TELNET_OPTION 251756938
#define CURL_TELNET_OPTION_SYNTAX 251756946
#define CURL_OBSOLETE50 251756954
#define CURL_PEER_FAILED_VERIF 251756962
#define CURL_GOT_NOTHING 251756970
#define CURL_SSL_ENGINE_NOTFOUND 251756978
#define CURL_SSL_ENGINE_SETFAILED 251756986
#define CURL_SEND_ERROR 251756994
#define CURL_RECV_ERROR 251757002
#define CURL_OBSOLETE57 251757010
#define CURL_SSL_CERTPROBLEM 251757018
#define CURL_SSL_CIPHER 251757026
#define CURL_SSL_CACERT 251757034
#define CURL_BAD_CONTENT_ENCODING 251757042
#define CURL_LDAP_INVALID_URL 251757050
#define CURL_FILESIZE_EXCEEDED 251757058
#define CURL_USE_SSL_FAILED 251757066
#define CURL_SEND_FAIL_REWIND 251757074
#define CURL_SSL_ENGINE_INITFAILED 251757082
#define CURL_LOGIN_DENIED 251757090
#define CURL_TFTP_NOTFOUND 251757098
#define CURL_TFTP_PERM 251757106
#define CURL_REMOTE_DISK_FULL 251757114
#define CURL_TFTP_ILLEGAL 251757122
#define CURL_TFTP_UNKNOWNID 251757130
#define CURL_REMOTE_FILE_EXISTS 251757138
#define CURL_TFTP_NOSUCHUSER 251757146
#define CURL_CONV_FAILED 251757154
#define CURL_CONV_REQD 251757162
#define CURL_SSL_CACERT_BADFILE 251757170
#define CURL_REMOTE_FILE_NOT_FOUND 251757178
#define CURL_SSH 251757186
#define CURL_SSL_SHUTDOWN_FAILED 251757194
#define CURL_AGAIN 251757202
#define CURL_SSL_CRL_BADFILE 251757210
#define CURL_SSL_ISSUER_ERROR 251757218
#define CURL_CURL_LAST 251757226
 
#pragma __member_alignment __restore

#endif /* HEADER_CURLMSG_H */
