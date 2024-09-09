/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @addtogroup Web
 * @{
 *
 * @brief Provides APIs for the ArkWeb net errors.
 * @since 12
 */
/**
 * @file arkweb_net_error_list.h
 *
 * @brief Declares the APIs for the ArkWeb net errors.
 * @library libohweb.so
 * @syscap SystemCapability.Web.Webview.Core
 * @since 12
 */
#ifndef ARKWEB_NET_ERROR_LIST_H
#define ARKWEB_NET_ERROR_LIST_H

enum ArkWeb_NetError {
    /*
     * @brief Normal.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_NET_OK = 0,

    /*
     * @brief An asynchronous IO operation is not yet complete.  This usually does not
     *        indicate a fatal error.  Typically this error will be generated as a
     *        notification to wait for some external notification that the IO operation
     *        finally completed.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_IO_PENDING = -1,

    /*
     * @brief A generic failure occurred.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_FAILED = -2,

    /*
     * @brief An operation was aborted.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_ABORTED = -3,

    /*
     * @brief An argument to the function is incorrect.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_INVALID_ARGUMENT = -4,

    /*
     * @brief The handle or file descriptor is invalid.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_INVALID_HANDLE = -5,

    /*
     * @brief The file or directory cannot be found.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_FILE_NOT_FOUND = -6,

    /*
     * @brief An operation timed out.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_TIMED_OUT = -7,

    /*
     * @brief The file is too large.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_FILE_TOO_LARGE = -8,

    /*
     * @brief An unexpected error.  This may be caused by a programming mistake or an
     *        invalid assumption.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_UNEXPECTED = -9,

    /*
     * @brief Permission to access a resource, other than the network, was denied.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_ACCESS_DENIED = -10,

    /*
     * @brief The operation failed because of unimplemented functionality.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_NOT_IMPLEMENTED = -11,

    /*
     * @brief There were not enough resources to complete the operation.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_INSUFFICIENT_RESOURCES = -12,
    
    /*
     * @brief Memory allocation failed.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_OUT_OF_MEMORY = -13,

    /*
     * @brief The file upload failed because the file's modification time was different
     *        from the expectation.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_UPLOAD_FILE_CHANGED = -14,

    /*
     * @brief The socket is not connected.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_SOCKET_NOT_CONNECTED = -15,

    /*
     * @brief The file already exists.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_FILE_EXISTS = -16,

    /*
     * @brief The path or file name is too long.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_FILE_PATH_TOO_LONG = -17,

    /*
     * @brief Not enough room left on the disk.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_FILE_NO_SPACE = -18,

    /*
     * @brief The file has a virus.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_FILE_VIRUS_INFECTED = -19,

    /*
     * @brief The client chose to block the request.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_BLOCKED_BY_CLIENT = -20,
    
    /*
     * @brief The network changed.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_NETWORK_CHANGED = -21,

    /*
     * @brief The request was blocked by the URL block list configured by the domain
     *        administrator.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_BLOCKED_BY_ADMINISTRATOR = -22,

    /*
     * @brief The socket is already connected.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_SOCKET_CONNECTED = -23,
    
    /*
     * @brief The upload failed because the upload stream needed to be re-read, due to a
     *        retry or a redirect, but the upload stream doesn't support that operation.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_UPLOAD_STREAM_REWIND_NOT_SUPPORTED = -25,

    /*
     * @brief The request failed because the URLRequestContext is shutting down, or has
     *        been shut down.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_CONTEXT_SHUT_DOWN = -26,
    
    /*
     * @brief The request failed because the response was delivered along with requirements
     *        which are not met ('X-Frame-Options' and 'Content-Security-Policy' ancestor
     *        checks and 'Cross-Origin-Resource-Policy' for instance,.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_BLOCKED_BY_RESPONSE = -27,

    /*
     * @brief The request was blocked by system policy disallowing some or all cleartext
     *        requests.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_CLEARTEXT_NOT_PERMITTED = -29,
    
    /*
     * @brief The request was blocked by a Content Security Policy.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_BLOCKED_BY_CSP = -30,

    /*
     * @brief The request was blocked because of no H/2 or QUIC session.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_H2_OR_QUIC_REQUIRED = -31,

    /*
     * @brief The request was blocked by CORB or ORB.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_BLOCKED_BY_ORB = -32,

    /*
     * @brief A connection was closed (corresponding to a TCP FIN,.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_CONNECTION_CLOSED = -100,

    /*
     * @brief A connection was reset (corresponding to a TCP RST,.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_CONNECTION_RESET = -101,

    /*
     * @brief A connection attempt was refused.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_CONNECTION_REFUSED = -102,
    
    /*
     * @brief A connection timed out as a result of not receiving an ACK for data sent.
     *        This can include a FIN packet that did not get ACK'd.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_CONNECTION_ABORTED = -103,

    /*
     * @brief A connection attempt failed.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_CONNECTION_FAILED = -104,

    /*
     * @brief The host name could not be resolved.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_NAME_NOT_RESOLVED = -105,

    /*
     * @brief The Internet connection has been lost.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_INTERNET_DISCONNECTED = -106,

    /*
     * @brief An SSL protocol error occurred.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_SSL_PROTOCOL_ERROR = -107,

    /*
     * @brief The IP address or port number is invalid (e.g., cannot connect to the IP
     *        address 0 or the port 0,.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_ADDRESS_INVALID = -108,

    /*
     * @brief The IP address is unreachable.  This usually means that there is no route to
     *        the specified host or network.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_ADDRESS_UNREACHABLE = -109,

    /*
     * @brief The server requested a client certificate for SSL client authentication.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_SSL_CLIENT_AUTH_CERT_NEEDED = -110,

    /*
     * @brief A tunnel connection through the proxy could not be established.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_TUNNEL_CONNECTION_FAILED = -111,

    /*
     * @brief No SSL protocol versions are enabled.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_NO_SSL_VERSIONS_ENABLED = -112,

    /*
     * @brief The client and server don't support a common SSL protocol version or
     *        cipher suite.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_SSL_VERSION_OR_CIPHER_MISMATCH = -113,

    /*
     * @brief The server requested a renegotiation (rehandshake,.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_SSL_RENEGOTIATION_REQUESTED = -114,

    /*
     * @brief The proxy requested authentication (for tunnel establishment, with an
     *        unsupported method.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_PROXY_AUTH_UNSUPPORTED = -115,

    /*
     * @brief The SSL handshake failed because of a bad or missing client certificate.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_BAD_SSL_CLIENT_AUTH_CERT = -117,

    /*
     * @brief A connection attempt timed out.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_CONNECTION_TIMED_OUT = -118,

    /*
     * @brief There are too many pending DNS resolves, so a request in the queue was
     *        aborted.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_HOST_RESOLVER_QUEUE_TOO_LARGE = -119,

    /*
     * @brief Failed establishing a connection to the SOCKS proxy server for a target host.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_SOCKS_CONNECTION_FAILED = -120,

    /*
     * @brief The SOCKS proxy server failed establishing connection to the target host
     *        because that host is unreachable.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_SOCKS_CONNECTION_HOST_UNREACHABLE = -121,

    /*
     * @brief The request to negotiate an alternate protocol failed.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_ALPN_NEGOTIATION_FAILED = -122,

    /*
     * @brief The peer sent an SSL no_renegotiation alert message.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_SSL_NO_RENEGOTIATION = -123,

    /*
     * @brief Winsock sometimes reports more data written than passed.  This is probably
     *        due to a broken LSP.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_WINSOCK_UNEXPECTED_WRITTEN_BYTES = -124,

    /*
     * @brief An SSL peer sent us a fatal decompression_failure alert. This typically
     *        occurs when a peer selects DEFLATE compression in the mistaken belief that
     *        it supports it.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_SSL_DECOMPRESSION_FAILURE_ALERT = -125,

    /*
     * @brief An SSL peer sent us a fatal bad_record_mac alert. This has been observed
     *        from servers with buggy DEFLATE support.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_SSL_BAD_RECORD_MAC_ALERT = -126,

    /*
     * @brief The proxy requested authentication (for tunnel establishment,.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_PROXY_AUTH_REQUESTED = -127,

    /*
     * @brief Could not create a connection to the proxy server. An error occurred
     *        either in resolving its name, or in connecting a socket to it.
     *        Note that this does NOT include failures during the actual "CONNECT" method
     *        of an HTTP proxy.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_PROXY_CONNECTION_FAILED = -130,

    /*
     * @brief A mandatory proxy configuration could not be used. Currently this means
     *        that a mandatory PAC script could not be fetched, parsed or executed.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_MANDATORY_PROXY_CONFIGURATION_FAILED = -131,

    /*
     * @brief We've hit the max socket limit for the socket pool while preconnecting.  We
     *        don't bother trying to preconnect more sockets.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_PRECONNECT_MAX_SOCKET_LIMIT = -133,

    /*
     * @brief The permission to use the SSL client certificate's private key was denied.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_SSL_CLIENT_AUTH_PRIVATE_KEY_ACCESS_DENIED = -134,

    /*
     * @brief The SSL client certificate has no private key.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_SSL_CLIENT_AUTH_CERT_NO_PRIVATE_KEY = -135,

    /*
     * @brief The certificate presented by the HTTPS Proxy was invalid.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_PROXY_CERTIFICATE_INVALID = -136,

    /*
     * @brief An error occurred when trying to do a name resolution (DNS,.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_NAME_RESOLUTION_FAILED = -137,

    /*
     * @brief Permission to access the network was denied. This is used to distinguish
     *        errors that were most likely caused by a firewall from other access denied
     *        errors. See also ERR_ACCESS_DENIED.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_NETWORK_ACCESS_DENIED = -138,

    /*
     * @brief The request throttler module cancelled this request to avoid DDOS.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_TEMPORARILY_THROTTLED = -139,
 
    /*
     * @brief A request to create an SSL tunnel connection through the HTTPS proxy
     *        received a 302 (temporary redirect, response.  The response body might
     *        include a description of why the request failed.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_HTTPS_PROXY_TUNNEL_RESPONSE_REDIRECT = -140,

    /*
     * @brief We were unable to sign the CertificateVerify data of an SSL client auth
     *        handshake with the client certificate's private key.
     *        Possible causes for this include the user implicitly or explicitly
     *        denying access to the private key, the private key may not be valid for
     *        signing, the key may be relying on a cached handle which is no longer
     *        valid, or the CSP won't allow arbitrary data to be signed.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_SSL_CLIENT_AUTH_SIGNATURE_FAILED = -141,

    /*
     * @brief The message was too large for the transport.  (for example a UDP message
     *        which exceeds size threshold,.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_MSG_TOO_BIG = -142,

    /*
     * @brief Websocket protocol error. Indicates that we are terminating the connection
     *        due to a malformed frame or other protocol violation.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_WS_PROTOCOL_ERROR = -145,

    /*
     * @brief Returned when attempting to bind an address that is already in use.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_ADDRESS_IN_USE = -147,

    /*
     * @brief An operation failed because the SSL handshake has not completed.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_SSL_HANDSHAKE_NOT_COMPLETED = -148,

    /*
     * @brief SSL peer's public key is invalid.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_SSL_BAD_PEER_PUBLIC_KEY = -149,

    /*
     * @brief The certificate didn't match the built-in public key pins for the host name.
     *        The pins are set in net/http/transport_security_state.cc and require that
     *        one of a set of public keys exist on the path from the leaf to the root.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_SSL_PINNED_KEY_NOT_IN_CERT_CHAIN = -150,

    /*
     * @brief Server request for client certificate did not contain any types we support.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_CLIENT_AUTH_CERT_TYPE_UNSUPPORTED = -151,

    /*
     * @brief An SSL peer sent us a fatal decrypt_error alert. This typically occurs when
     *        a peer could not correctly verify a signature (in CertificateVerify or
     *        ServerKeyExchange, or validate a Finished message.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_SSL_DECRYPT_ERROR_ALERT = -153,
    
    /*
     * @brief There are too many pending WebSocketJob instances, so the new job was not
     *        pushed to the queue.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_WS_THROTTLE_QUEUE_TOO_LARGE = -154,
        
    /*
     * @brief The SSL server certificate changed in a renegotiation.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_SSL_SERVER_CERT_CHANGED = -156,

    /*
     * @brief The SSL server sent us a fatal unrecognized_name alert.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_SSL_UNRECOGNIZED_NAME_ALERT = -159,

    /*
     * @brief Failed to set the socket's receive buffer size as requested.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_SOCKET_SET_RECEIVE_BUFFER_SIZE_ERROR = -160,

    /*
     * @brief Failed to set the socket's send buffer size as requested.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_SOCKET_SET_SEND_BUFFER_SIZE_ERROR = -161,

    /*
     * @brief Failed to set the socket's receive buffer size as requested, despite success
     *        return code from setsockopt.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_SOCKET_RECEIVE_BUFFER_SIZE_UNCHANGEABLE = -162,

    /*
     * @brief Failed to set the socket's send buffer size as requested, despite success
     *        return code from setsockopt.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_SOCKET_SEND_BUFFER_SIZE_UNCHANGEABLE = -163,

    /*
     * @brief Failed to import a client certificate from the platform store into the SSL
     *        library.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_SSL_CLIENT_AUTH_CERT_BAD_FORMAT = -164,

    /*
     * @brief Resolving a hostname to an IP address list included the IPv4 address
     *        "127.0.53.53". This is a special IP address which ICANN has recommended to
     *        indicate there was a name collision, and alert admins to a potential
     *        problem.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_ICANN_NAME_COLLISION = -166,

    /*
     * @brief The SSL server presented a certificate which could not be decoded. This is
     *        not a certificate error code as no X509Certificate object is available. This
     *        error is fatal.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_SSL_SERVER_CERT_BAD_FORMAT = -167,

    /*
     * @brief Certificate Transparency: Received a signed tree head that failed to parse.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_CT_STH_PARSING_FAILED = -168,

    /*
     * @brief Certificate Transparency: Received a signed tree head whose JSON parsing was
     *        OK but was missing some of the fields.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_CT_STH_INCOMPLETE = -169,

    /*
     * @brief The attempt to reuse a connection to send proxy auth credentials failed
     *        before the AuthController was used to generate credentials. The caller should
     *        reuse the controller with a new connection. This error is only used
     *        internally by the network stack.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_UNABLE_TO_REUSE_CONNECTION_FOR_PROXY_AUTH = -170,

    /*
     * @brief Certificate Transparency: Failed to parse the received consistency proof.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_CT_CONSISTENCY_PROOF_PARSING_FAILED = -171,

    /*
     * @brief The SSL server required an unsupported cipher suite that has since been
     *        removed. This error will temporarily be signaled on a fallback for one or two
     *        releases immediately following a cipher suite's removal, after which the
     *        fallback will be removed.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_SSL_OBSOLETE_CIPHER = -172,

    /*
     * @brief When a WebSocket handshake is done successfully and the connection has been
     *        upgraded, the URLRequest is cancelled with this error code.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_WS_UPGRADE = -173,

    /*
     * @brief Socket ReadIfReady support is not implemented. This error should not be user
     *        visible, because the normal Read(, method is used as a fallback.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_READ_IF_READY_NOT_IMPLEMENTED = -174,

    /*
     * @brief No socket buffer space is available.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_NO_BUFFER_SPACE = -176,

    /*
     * @brief There were no common signature algorithms between our client certificate
     *        private key and the server's preferences.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_SSL_CLIENT_AUTH_NO_COMMON_ALGORITHMS = -177,

    /*
     * @brief TLS 1.3 early data was rejected by the server. This will be received before
     *        any data is returned from the socket. The request should be retried with
     *        early data disabled.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_EARLY_DATA_REJECTED = -178,

    /*
     * @brief TLS 1.3 early data was offered, but the server responded with TLS 1.2 or
     *        earlier. This is an internal error code to account for a
     *        backwards-compatibility issue with early data and TLS 1.2. It will be
     *        received before any data is returned from the socket. The request should be
     *        retried with early data disabled.
     *        See https://tools.ietf.org/html/rfc8446#appendix-D.3 for details.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_WRONG_VERSION_ON_EARLY_DATA = -179,

    /*
     * @brief TLS 1.3 was enabled, but a lower version was negotiated and the server
     *        returned a value indicating it supported TLS 1.3. This is part of a security
     *        check in TLS 1.3, but it may also indicate the user is behind a buggy
     *        TLS-terminating proxy which implemented TLS 1.2 incorrectly. (See
     *        rhttps://crbug.com/boringssl/226.,
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_TLS13_DOWNGRADE_DETECTED = -180,

    /*
     * @brief The server's certificate has a keyUsage extension incompatible with the
     *        negotiated TLS key exchange method.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_SSL_KEY_USAGE_INCOMPATIBLE = -181,

    /*
     * @brief The ECHConfigList fetched over DNS cannot be parsed.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_INVALID_ECH_CONFIG_LIST = -182,

    /*
     * @brief ECH was enabled, but the server was unable to decrypt the encrypted
     *        ClientHello.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_ECH_NOT_NEGOTIATED = -183,

    /*
     * @brief ECH was enabled, the server was unable to decrypt the encrypted ClientHello,
     *        and additionally did not present a certificate valid for the public name.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_ECH_FALLBACK_CERTIFICATE_INVALID = -184,

    /*
     * @brief The server responded with a certificate whose common name did not match
     *        the host name.  This could mean:
     *        1. An attacker has redirected our traffic to their server and is
     *           presenting a certificate for which they know the private key.
     *        2. The server is misconfigured and responding with the wrong cert.
     *        3. The user is on a wireless network and is being redirected to the
     *           network's login page.
     *        4. The OS has used a DNS search suffix and the server doesn't have
     *           a certificate for the abbreviated name in the address bar.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_CERT_COMMON_NAME_INVALID = -200,

    /*
     * @brief The server responded with a certificate that, by our clock, appears to
     *        either not yet be valid or to have expired.  This could mean:
     *        1. An attacker is presenting an old certificate for which they have
     *           managed to obtain the private key.
     *        2. The server is misconfigured and is not presenting a valid cert.
     *        3. Our clock is wrong.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_CERT_DATE_INVALID = -201,

    /*
     * @brief The server responded with a certificate that is signed by an authority
     *        we don't trust.  The could mean:
     *        1. An attacker has substituted the real certificate for a cert that
     *           contains their public key and is signed by their cousin.
     *        2. The server operator has a legitimate certificate from a CA we don't
     *           know about, but should trust.
     *        3. The server is presenting a self-signed certificate, providing no
     *           defense against active attackers (but foiling passive attackers,.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_CERT_AUTHORITY_INVALID = -202,

    /*
     * @brief The server responded with a certificate that contains errors.
     *        This error is not recoverable.
     *        MSDN describes this error as follows:
     *           "The SSL certificate contains errors."
     *        NOTE: It's unclear how this differs from ERR_CERT_INVALID. For consistency,
     *        use that code instead of this one from now on.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_CERT_CONTAINS_ERRORS = -203,

    /*
     * @brief The certificate has no mechanism for determining if it is revoked.  In
     *        effect, this certificate cannot be revoked.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_CERT_NO_REVOCATION_MECHANISM = -204,

    /*
     * @brief Revocation information for the security certificate for this site is not
     *        available.  This could mean:
     *        1. An attacker has compromised the private key in the certificate and is
     *           blocking our attempt to find out that the cert was revoked.
     *        2. The certificate is unrevoked, but the revocation server is busy or
     *           unavailable.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_CERT_UNABLE_TO_CHECK_REVOCATION = -205,

    /*
     * @brief The server responded with a certificate has been revoked.
     *        We have the capability to ignore this error, but it is probably not the
     *        thing to do.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_CERT_REVOKED = -206,

    /*
     * @brief The server responded with a certificate that is invalid.
     *        This error is not recoverable.
     *        MSDN describes this error as follows:
     *           "The SSL certificate is invalid."
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_CERT_INVALID = -207,

    /*
     * @brief The server responded with a certificate that is signed using a weak
     *        signature algorithm.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_CERT_WEAK_SIGNATURE_ALGORITHM = -208,

    /*
     * @brief The host name specified in the certificate is not unique.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_CERT_NON_UNIQUE_NAME = -210,

    /*
     * @brief The server responded with a certificate that contains a weak key (e.g.
     *        a too-small RSA key,.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_CERT_WEAK_KEY = -211,
    
    /*
     * @brief The certificate claimed DNS names that are in violation of name constraints.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_CERT_NAME_CONSTRAINT_VIOLATION = -212,

    /*
     * @brief The certificate's validity period is too long.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_CERT_VALIDITY_TOO_LONG = -213,

    /*
     * @brief Certificate Transparency was required for this connection, but the server
     *        did not provide CT information that complied with the policy.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_CERTIFICATE_TRANSPARENCY_REQUIRED = -214,

    /*
     * @brief The certificate chained to a legacy Symantec root that is no longer trusted.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_CERT_SYMANTEC_LEGACY = -215,

    /*
     * @brief The certificate is known to be used for interception by an entity other
     *        the device owner.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_CERT_KNOWN_INTERCEPTION_BLOCKED = -217,

    /*
     * @brief The connection uses an obsolete version of SSL/TLS or cipher.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_SSL_OBSOLETE_VERSION_OR_CIPHER = -218,

    /*
     * @brief The value immediately past the last certificate error code.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_CERT_END = -219,

    /*
     * @brief The URL is invalid.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_INVALID_URL = -300,

    /*
     * @brief The scheme of the URL is disallowed.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_DISALLOWED_URL_SCHEME = -301,

    /*
     * @brief The scheme of the URL is unknown.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_UNKNOWN_URL_SCHEME = -302,

    /*
     * @brief Attempting to load an URL resulted in a redirect to an invalid URL.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_INVALID_REDIRECT = -303,

    /*
     * @brief Attempting to load an URL resulted in too many redirects.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_TOO_MANY_REDIRECTS = -310,

    /*
     * @brief Attempting to load an URL resulted in an unsafe redirect (e.g., a redirect
     *        to file:// is considered unsafe,.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_UNSAFE_REDIRECT = -311,
    
    /*
     * @brief Attempting to load an URL with an unsafe port number.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_UNSAFE_PORT = -312,

    /*
     * @brief The server's response was invalid.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_INVALID_RESPONSE = -320,

    /*
     * @brief Error in chunked transfer encoding.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_INVALID_CHUNKED_ENCODING = -321,

    /*
     * @brief The server did not support the request method.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_METHOD_UNSUPPORTED = -322,

    /*
     * @brief The response was 407 (Proxy Authentication Required,, yet we did not send
     *        the request to a proxy.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_UNEXPECTED_PROXY_AUTH = -323,

    /*
     * @brief The server closed the connection without sending any data.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_EMPTY_RESPONSE = -324,

    /*
     * @brief The headers section of the response is too large.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_RESPONSE_HEADERS_TOO_BIG = -325,
    
    /*
     * @brief The evaluation of the PAC script failed.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_PAC_SCRIPT_FAILED = -327,

    /*
     * @brief The response was 416 (Requested range not satisfiable, and the server cannot
     *        satisfy the range requested.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_REQUEST_RANGE_NOT_SATISFIABLE = -328,

    /*
     * @brief The identity used for authentication is invalid.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_MALFORMED_IDENTITY = -329,

    /*
     * @brief Content decoding of the response body failed.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_CONTENT_DECODING_FAILED = -330,

    /*
     * @brief An operation could not be completed because all network IO
     *        is suspended.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_NETWORK_IO_SUSPENDED = -331,

    /*
     * @brief FLIP data received without receiving a SYN_REPLY on the stream.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_SYN_REPLY_NOT_RECEIVED = -332,

    /*
     * @brief Converting the response to target encoding failed.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_ENCODING_CONVERSION_FAILED = -333,

    /*
     * @brief The server sent an FTP directory listing in a format we do not understand.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_UNRECOGNIZED_FTP_DIRECTORY_LISTING_FORMAT = -334,

    /*
     * @brief There are no supported proxies in the provided list.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_NO_SUPPORTED_PROXIES = -336,

    /*
     * @brief There is an HTTP/2 protocol error.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_HTTP2_PROTOCOL_ERROR = -337,

    /*
     * @brief Credentials could not be established during HTTP Authentication.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_INVALID_AUTH_CREDENTIALS = -338,

    /*
     * @brief An HTTP Authentication scheme was tried which is not supported on this
     *        machine.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_UNSUPPORTED_AUTH_SCHEME = -339,

    /*
     * @brief Detecting the encoding of the response failed.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_ENCODING_DETECTION_FAILED = -340,

    /*
     * @brief (GSSAPI, No Kerberos credentials were available during HTTP Authentication.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_MISSING_AUTH_CREDENTIALS = -341,

    /*
     * @brief An unexpected, but documented, SSPI or GSSAPI status code was returned.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_UNEXPECTED_SECURITY_LIBRARY_STATUS = -342,

    /*
     * @brief The environment was not set up correctly for authentication (for
     *        example, no KDC could be found or the principal is unknown.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_MISCONFIGURED_AUTH_ENVIRONMENT = -343,

    /*
     * @brief An undocumented SSPI or GSSAPI status code was returned.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_UNDOCUMENTED_SECURITY_LIBRARY_STATUS = -344,

    /*
     * @brief The HTTP response was too big to drain.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_RESPONSE_BODY_TOO_BIG_TO_DRAIN = -345,

    /*
     * @brief The HTTP response contained multiple distinct Content-Length headers.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_RESPONSE_HEADERS_MULTIPLE_CONTENT_LENGTH = -346,

    /*
     * @brief HTTP/2 headers have been received, but not all of them - status or version
     *        headers are missing, so we're expecting additional frames to complete them.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_INCOMPLETE_HTTP2_HEADERS = -347,

    /*
     * @brief No PAC URL configuration could be retrieved from DHCP. This can indicate
     *        either a failure to retrieve the DHCP configuration, or that there was no
     *        PAC URL configured in DHCP.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_PAC_NOT_IN_DHCP = -348,

    /*
     * @brief The HTTP response contained multiple Content-Disposition headers.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_RESPONSE_HEADERS_MULTIPLE_CONTENT_DISPOSITION = -349,

    /*
     * @brief The HTTP response contained multiple Location headers.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_RESPONSE_HEADERS_MULTIPLE_LOCATION = -350,

    /*
     * @brief HTTP/2 server refused the request without processing, and sent either a
     *        GOAWAY frame with error code NO_ERROR and Last-Stream-ID lower than the
     *        stream id corresponding to the request indicating that this request has not
     *        been processed yet, or a RST_STREAM frame with error code REFUSED_STREAM.
     *        Client MAY retry (on a different connection,.  See RFC7540 Section 8.1.4.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_HTTP2_SERVER_REFUSED_STREAM = -351,

    /*
     * @brief HTTP/2 server didn't respond to the PING message.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_HTTP2_PING_FAILED = -352,

    /*
     * @brief The HTTP response body transferred fewer bytes than were advertised by the
     *        Content-Length header when the connection is closed.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_CONTENT_LENGTH_MISMATCH = -354,

    /*
     * @brief The HTTP response body is transferred with Chunked-Encoding, but the
     *        terminating zero-length chunk was never sent when the connection is closed.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_INCOMPLETE_CHUNKED_ENCODING = -355,

    /*
     * @brief There is a QUIC protocol error.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_QUIC_PROTOCOL_ERROR = -356,

    /*
     * @brief The HTTP headers were truncated by an EOF.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_RESPONSE_HEADERS_TRUNCATED = -357,
 
    /*
     * @brief The QUIC crypto handshake failed.  This means that the server was unable
     *        to read any requests sent, so they may be resent.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_QUIC_HANDSHAKE_FAILED = -358,

    /*
     * @brief Transport security is inadequate for the HTTP/2 version.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_HTTP2_INADEQUATE_TRANSPORT_SECURITY = -360,

    /*
     * @brief The peer violated HTTP/2 flow control.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_HTTP2_FLOW_CONTROL_ERROR = -361,

    /*
     * @brief The peer sent an improperly sized HTTP/2 frame.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_HTTP2_FRAME_SIZE_ERROR = -362,

    /*
     * @brief Decoding or encoding of compressed HTTP/2 headers failed.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_HTTP2_COMPRESSION_ERROR = -363,

    /*
     * @brief Proxy Auth Requested without a valid Client Socket Handle.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_PROXY_AUTH_REQUESTED_WITH_NO_CONNECTION = -364,

    /*
     * @brief HTTP_1_1_REQUIRED error code received on HTTP/2 session.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_HTTP_1_1_REQUIRED = -365,

    /*
     * @brief HTTP_1_1_REQUIRED error code received on HTTP/2 session to proxy.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_PROXY_HTTP_1_1_REQUIRED = -366,

    /*
     * @brief The PAC script terminated fatally and must be reloaded.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_PAC_SCRIPT_TERMINATED = -367,

    /*
     * @brief The server was expected to return an HTTP/1.x response, but did not. Rather
     *        than treat it as HTTP/0.9, this error is returned.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_INVALID_HTTP_RESPONSE = -370,

    /*
     * @brief Initializing content decoding failed.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_CONTENT_DECODING_INIT_FAILED = -371,

    /*
     * @brief Received HTTP/2 RST_STREAM frame with NO_ERROR error code.  This error should
     *        be handled internally by HTTP/2 code, and should not make it above the
     *        SpdyStream layer.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_HTTP2_RST_STREAM_NO_ERROR_RECEIVED = -372,

    /*
     * @brief The pushed stream claimed by the request is no longer available.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_HTTP2_PUSHED_STREAM_NOT_AVAILABLE = -373,

    /*
     * @brief A pushed stream was claimed and later reset by the server. When this happens,
     *        the request should be retried.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_HTTP2_CLAIMED_PUSHED_STREAM_RESET_BY_SERVER = -374,

    /*
     * @brief An HTTP transaction was retried too many times due for authentication or
     *        invalid certificates.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_TOO_MANY_RETRIES = -375,

    /*
     * @brief Received an HTTP/2 frame on a closed stream.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_HTTP2_STREAM_CLOSED = -376,

    /*
     * @brief Client is refusing an HTTP/2 stream.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_HTTP2_CLIENT_REFUSED_STREAM = -377,

    /*
     * @brief A pushed HTTP/2 stream was claimed by a request based on matching URL and
     *        request headers, but the pushed response headers do not match the request.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_HTTP2_PUSHED_RESPONSE_DOES_NOT_MATCH = -378,

    /*
     * @brief The server returned a non-2xx HTTP response code.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_HTTP_RESPONSE_CODE_FAILURE = -379,

    /*
     * @brief The certificate presented on a QUIC connection does not chain to a known root
     *        and the origin connected to is not on a list of domains where unknown roots
     *        are allowed.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_QUIC_UNKNOWN_CERT_ROOT = -380,

    /*
     * @brief A GOAWAY frame has been received indicating that the request has not been
     *        processed and is therefore safe to retry on a different connection.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_QUIC_GOAWAY_REQUEST_CAN_BE_RETRIED = -381,

    /*
     * @brief The ACCEPT_CH restart has been triggered too many times.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_TOO_MANY_ACCEPT_CH_RESTARTS = -382,

    /*
     * @brief The IP address space of the remote endpoint differed from the previous
     *        observed value during the same request. Any cache entry for the affected
     *        request should be invalidated.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_INCONSISTENT_IP_ADDRESS_SPACE = -383,

    /*
     * @brief The IP address space of the cached remote endpoint is blocked by local
     *        network access check.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_CACHED_IP_ADDRESS_SPACE_BLOCKED_BY_LOCAL_NETWORK_ACCESS_POLICY = -384,

    /*
     * @brief The cache does not have the requested entry.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_CACHE_MISS = -400,

    /*
     * @brief Unable to read from the disk cache.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_CACHE_READ_FAILURE = -401,

    /*
     * @brief Unable to write to the disk cache.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_CACHE_WRITE_FAILURE = -402,

    /*
     * @brief The operation is not supported for this entry.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_CACHE_OPERATION_UNSUPPORTED = -403,

    /*
     * @brief The disk cache is unable to open this entry.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_CACHE_OPEN_FAILURE = -404,

    /*
     * @brief The disk cache is unable to create this entry.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_CACHE_CREATE_FAILURE = -405,

    /*
     * @brief Multiple transactions are racing to create disk cache entries.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_CACHE_RACE = -406,

    /*
     * @brief The cache was unable to read a checksum record on an entry.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_CACHE_CHECKSUM_READ_FAILURE = -407,

    /*
     * @brief The cache found an entry with an invalid checksum.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_CACHE_CHECKSUM_MISMATCH = -408,

    /*
     * @brief Internal error code for the HTTP cache.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_CACHE_LOCK_TIMEOUT = -409,

    /*
     * @brief Received a challenge after the transaction has read some data, and the
     *        credentials aren't available.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_CACHE_AUTH_FAILURE_AFTER_READ = -410,

    /*
     * @brief Internal not-quite error code for the HTTP cache.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_CACHE_ENTRY_NOT_SUITABLE = -411,

    /*
     * @brief The disk cache is unable to doom this entry.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_CACHE_DOOM_FAILURE = -412,

    /*
     * @brief The disk cache is unable to open or create this entry.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_CACHE_OPEN_OR_CREATE_FAILURE = -413,

    /*
     * @brief The server's response was insecure (e.g. there was a cert error,.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_INSECURE_RESPONSE = -501,

    /*
     * @brief An attempt to import a client certificate failed, as the user's key
     *        database lacked a corresponding private key.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_NO_PRIVATE_KEY_FOR_CERT = -502,

    /*
     * @brief An error adding a certificate to the OS certificate database.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_ADD_USER_CERT_FAILED = -503,

    /*
     * @brief An error occurred while handling a signed exchange.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_INVALID_SIGNED_EXCHANGE = -504,

    /*
     * @brief An error occurred while handling a Web Bundle source.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_INVALID_WEB_BUNDLE = -505,

    /*
     * @brief A Trust Tokens protocol operation-executing request failed for one of a
     *        number of reasons (precondition failure, internal error, bad response,.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_TRUST_TOKEN_OPERATION_FAILED = -506,

    /*
     * @brief When handling a Trust Tokens protocol operation-executing request, the system
     *        was able to execute the request's Trust Tokens operation without sending the
     *        request to its destination.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_TRUST_TOKEN_OPERATION_SUCCESS_WITHOUT_SENDING_REQUEST = -507,

    /*
     * @brief A generic error for failed FTP control connection command.
     *        If possible, please use or add a more specific error code.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_FTP_FAILED = -601,

    /*
     * @brief The server cannot fulfill the request at this point. This is a temporary error.
     *        FTP response code 421.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_FTP_SERVICE_UNAVAILABLE = -602,

    /*
     * @brief The server has aborted the transfer.
     *        FTP response code 426.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_FTP_TRANSFER_ABORTED = -603,

    /*
     * @brief The file is busy, or some other temporary error condition on opening the file.
     *        FTP response code 450.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_FTP_FILE_BUSY = -604,

    /*
     * @brief Server rejected our command because of syntax errors.
     *        FTP response codes 500, 501.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_FTP_SYNTAX_ERROR = -605,

    /*
     * @brief Server does not support the command we issued.
     *        FTP response codes 502, 504.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_FTP_COMMAND_UNSUPPORTED = -606,

    /*
     * @brief Server rejected our command because we didn't issue the commands in right order.
     *        FTP response code 503.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_FTP_BAD_COMMAND_SEQUENCE = -607,

    /*
     * @brief PKCS #12 import failed due to incorrect password.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_PKCS12_IMPORT_BAD_PASSWORD = -701,

    /*
     * @brief PKCS #12 import failed due to other error.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_PKCS12_IMPORT_FAILED = -702,

    /*
     * @brief CA import failed - not a CA cert.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_IMPORT_CA_CERT_NOT_CA = -703,

    /*
     * @brief Import failed - certificate already exists in database.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_IMPORT_CERT_ALREADY_EXISTS = -704,

    /*
     * @brief CA import failed due to some other error.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_IMPORT_CA_CERT_FAILED = -705,

    /*
     * @brief Server certificate import failed due to some internal error.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_IMPORT_SERVER_CERT_FAILED = -706,

    /*
     * @brief PKCS #12 import failed due to invalid MAC.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_PKCS12_IMPORT_INVALID_MAC = -707,

    /*
     * @brief PKCS #12 import failed due to invalid/corrupt file.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_PKCS12_IMPORT_INVALID_FILE = -708,

    /*
     * @brief PKCS #12 import failed due to unsupported features.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_PKCS12_IMPORT_UNSUPPORTED = -709,

    /*
     * @brief Key generation failed.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_KEY_GENERATION_FAILED = -710,

    /*
     * @brief Failure to export private key.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_PRIVATE_KEY_EXPORT_FAILED = -712,

    /*
     * @brief Self-signed certificate generation failed.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_SELF_SIGNED_CERT_GENERATION_FAILED = -713,

    /*
     * @brief The certificate database changed in some way.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_CERT_DATABASE_CHANGED = -714,

    /*
     * @brief The certificate verifier configuration changed in some way.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_CERT_VERIFIER_CHANGED = -716,

    /*
     * @brief DNS resolver received a malformed response.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_DNS_MALFORMED_RESPONSE = -800,

    /*
     * @brief DNS server requires TCP.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_DNS_SERVER_REQUIRES_TCP = -801,

    /*
     * @brief DNS server failed.  This error is returned for all of the following
     *        error conditions:
     *        1 - Format error - The name server was unable to interpret the query.
     *        2 - Server failure - The name server was unable to process this query
     *            due to a problem with the name server.
     *        4 - Not Implemented - The name server does not support the requested
     *            kind of query.
     *        5 - Refused - The name server refuses to perform the specified
     *            operation for policy reasons.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_DNS_SERVER_FAILED = -802,
 
    /*
     * @brief DNS transaction timed out.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_DNS_TIMED_OUT = -803,

    /*
     * @brief The entry was not found in cache or other local sources, for lookups where
     *        only local sources were queried.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_DNS_CACHE_MISS = -804,

    /*
     * @brief Suffix search list rules prevent resolution of the given host name.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_DNS_SEARCH_EMPTY = -805,

    /*
     * @brief Failed to sort addresses according to RFC3484.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_DNS_SORT_ERROR = -806,

    /*
     * @brief Failed to resolve the hostname of a DNS-over-HTTPS server.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_DNS_SECURE_RESOLVER_HOSTNAME_RESOLUTION_FAILED = -808,

    /*
     * @brief DNS identified the request as disallowed for insecure connection (http/ws,.
     *        Error should be handled as if an HTTP redirect was received to redirect to
     *        https or wss.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_DNS_NAME_HTTPS_ONLY = -809,

    /*
     * @brief All DNS requests associated with this job have been cancelled.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_DNS_REQUEST_CANCELED = -810,

    /*
     * @brief The hostname resolution of HTTPS record was expected to be resolved with
     *        alpn values of supported protocols, but did not.
     *
     * @syscap SystemCapability.Web.Webview.Core
     * @since 12
     */
    ARKWEB_ERR_DNS_NO_MATCHING_SUPPORTED_ALPN = -811,
};

#endif // ARKWEB_NET_ERROR_LIST_H
