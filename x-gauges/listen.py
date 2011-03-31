#!/usr/local/bin/python

"""
Note that a server must perform the sequence socket(), bind(), listen(),
accept() (possibly repeating the accept() to service more than one client),
while a client only needs the sequence socket(), connect().

Also note that the server does not send()/recv() on the socket it is listening
on but on the new socket returned by accept().
"""

import socket
import atexit
import time
import struct
import array
import string

TCP_SRVR    = False
receive_sz  = 10240 * 6
HOST        = "127.0.0.1"
PORT        = 35000              # Arbitrary non-privileged port
cnt         = 0
s           = None

def cleanup():
    print "\n...cleaning up..."
    if s:
        s.close()

def main():
    global cnt

    listening = True

    try:
        if TCP_SRVR:
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        else:
            s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

        s.bind((HOST, PORT))
        print "socket setup"
    except:
        print "Couldn't init the server"
        return

    print "listening on port: ", PORT

    buf = array.array('B', [0, 0, 0, 0])

    while listening:
        try:
            if TCP_SRVR: # TCP
                s.listen(1)
                conn, addr = s.accept()

                print "Client connected"
                data = conn.recv(receive_sz)

                if data:
                    print "Rceived data, count: \n", data.count('')
            else: # UDP
                msg, addr = s.recvfrom(receive_sz)
                cnt += 1
                print "Cnt: %d, Bytes received  : %d\n" % (cnt, msg.count(''))
#                print "Data 0: ", msg[0:4]
#                print "Data 1: ", msg[4:8]
#                print "Data 2: ", msg[8:12]
#                print "Data 3: ", msg[12:16]
#                print "Data 4: ", msg[16:20]
                print "systems_avionics_on          : ", struct.unpack_from("f", msg, offset=0)[0]
                print "systems_avionics_off         : ", struct.unpack_from("f", msg, offset=4)[0]
                print "b. compass_heading_deg_mag   : ", struct.unpack_from("f", msg, offset=8)[0]
                print "airspeed_kts                 : ", struct.unpack_from("f", msg, offset=12)[0]
                print "altitude_ft                  : ", struct.unpack_from("f", msg, offset=16)[0]
#                print "Data: ", msg[0:1]
#                print "\n2: ", atoi(msg[1],0x)
#                print "\n\n"

#            time.sleep(0.250)
        except KeyboardInterrupt:
            # Exit on any keyboard activity
            listening = False
"""
systems_avionics_on;
systems_avionics_off;
compass_heading_deg_mag;
airspeed_kts;
altitude_ft;
vvi_fpm;
turn_rate_roll_deg;
turn_rate_heading_deg;
heading_AHARS_deg_mag;
heading_electric_deg_mag;
heading_vacuum_deg_mag;
pitch_AHARS_deg;
pitch_electric_deg;
pitch_vacuum_deg;
radio_altimeter_height_ft;
radio_altimeter_dh_lit;
roll_AHARS_deg;
roll_electric_deg;
roll_vacuum_deg;
wind_heading_deg_mag;
wind_speed_kts;
"""
if __name__ == '__main__':
    atexit.register(cleanup)
    main()




