7주차

80포트가 기본 값
0~65535
ip4 4바이트 10진수 8bit  ipv6 128bit 16 16진수바이트
port 다양하게 서비스를 이용할 수 있음
unsignedint 양수만사용
bind server초기화
memset(&serv_addr,0,sizeof(serv_addr))
fmaily addr port
 네트워크 방식 빅 엔디안 통일된 방식
 빅 엔디안 리틀 엔디안 x 
 빅 엔디안 방식으로 통신
 구조체 프로토콜 주소체계 _addr ip주소 
 리틀엔디안이면 빅엔디안으로 변환해야 됨  long ip주소 32비트 
 포트는 16비트 shot
 숏트 16비트
 
 32비트 port
 
 aton a= String address n = Network address
