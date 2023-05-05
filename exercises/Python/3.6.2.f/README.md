# 3.6.2.f

```
        3.6.2.f: establish a secure communication channel using an SSL/TLS library
```

## Instructions


For this exercise you will be constructing a client that will connect to a 
server using TLS.

You will implement the function `connect_to_server` which will connect to a 
server on a port and use a Certificate Authority to validate that it is correct.

The client should then successfully connect to the valid server, or return 
errors for invalid server.

The valid server will send you a message to encrypt. You will encrypt that message
with the server's public key and the server will verify your message by
decrypting it with its private key.


All of your implementation should occur in 

`./src/__init__.py`

You should then submit your file in the src folder as your solution.
          

## CI Pipeline

Note: if your CI pipeline does not run when you commit, please ensure you have made a merge request to merge
your branch into the primary branch. This will trigger the rules for the CI Job configured for this item.

## Testing Locally

You can test your submission locally by running the 
following command from the root of the exercise folder

```
python3 tests/__init__.py
```


If you do run locally, you may need to install the requirements in requirements.txt with:

`pip3 install -r requirements.txt`
