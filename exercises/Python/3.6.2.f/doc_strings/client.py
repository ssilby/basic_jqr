def connect_to_server(cafile, hostname, port):
    """
    This function will connect to a server at the given hostname and port.

    You should use cafile as the Certificate Authority to validate the server's 
    certificate.

    If the certificate is valid, you should receive a message from the server.
    The message will be no more than 1024 bytes long.

    You should encrypt that message using the server's public key and send the 
    encrypted message back to the server. 

    If the certificate is not valid, you should terminate the connection 
    and raise an exception.
    """
