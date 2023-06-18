import app
import localeinfo
app.ServerName = None
host = '54.38.205.28'
STATE_NONE = '...'
		
STATE_DICT = {
	0 : '....',
	1 : 'NORM',
	2 : 'BUSY',
	3 : 'FULL'	}
SERVER1_CHANNEL_DICT = {
	1:{'key':11,'name':'CH1   ','ip':'54.38.205.28','tcp_port':30243,'udp_port':30243,'state':STATE_NONE,},
#	2:{'key':12,'name':'CH2   ','ip':'54.38.205.28','tcp_port':30257,'udp_port':30257,'state':STATE_NONE,},
#	3:{'key':13,'name':'CH3   ','ip':'54.38.205.28','tcp_port':30251,'udp_port':30251,'state':STATE_NONE,},
#	4:{'key':14,'name':'CH3   ','ip':'54.38.205.28','tcp_port':30255,'udp_port':30255,'state':STATE_NONE,},
}
REGION_NAME_DICT = {
	0 : 'Europe',
}
REGION_AUTH_SERVER_DICT = {
	0 : {
		1 : { 'ip':'54.38.205.28', 'port':30241, },
		
		}
}
REGION_DICT = {
	0 : {
		1 : { 'name' : 'Dev', 'channel' : SERVER1_CHANNEL_DICT, },
	},
}
MARKADDR_DICT = {
	10 : { 'ip' : '54.38.205.28', 'tcp_port' : 30243, 'mark' : '10.tga', 'symbol_path' : '10', },
	}
TESTADDR = { 'ip' : '54.38.205.28', 'tcp_port' : 30243, 'udp_port' : 30243, }