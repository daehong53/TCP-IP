import socket

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

svrIP = input(("Server ip(default: 127.0.0.1): "))
if svrIP == '':
    svrIP = '127.0.0.1'

port = input('port(default: 2500):')
if port == '':
    port == 2500
else:
    port = int(port)

sock.connect((svrIP, port))
print('Connected to ' + svrIP)

while True:
    msg = input("Sending message: ")

    if not msg:
            continue
    try:
            sock.send(msg.encode())

    except:
            print("연결이 종료되었습니다")
            break

    try:
        msg = sock.recv(1024)
        if not msg:
            print("연결이 종료되었습니다")
            break
        print(f'Received emssage: {msg.decode()}')

    except:
        print("연결이 종료되었습니다")
        break

sock.close()
