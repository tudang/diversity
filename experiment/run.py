#!/usr/bin/env python
import argparse
import subprocess
import shlex
import os
from threading import Timer
import time


def client(host, path, server_addr, output_dir):
    cmd = "ssh danghu@{0} {1}/client {2}".format(host, path, server_addr)
    print cmd
    with open('%s/client.txt' % output_dir, 'w') as out:
        ssh = subprocess.Popen(shlex.split(cmd),
                                stdout=out,
                                stderr=out,
                                shell=False)
    return ssh

def server(host, path, output_dir):
    cmd = "ssh danghu@{0} {1}/server".format(host, path)
    print cmd
    with open('%s/server.txt' % output_dir, 'w') as out:
        ssh = subprocess.Popen(shlex.split(cmd),
                                stdout=out,
                                stderr=out,
                                shell=False)
    return ssh


def kill_all(*pipes, **nodes):
    cmd = "ssh danghu@%s pkill server" % nodes['server']
    print cmd
    ssh = subprocess.Popen(shlex.split(cmd))
    ssh.wait()

    cmd = "ssh danghu@%s pkill client" % nodes['client']
    print cmd
    ssh = subprocess.Popen(shlex.split(cmd))
    ssh.wait()


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Run experiment.')
    parser.add_argument('--time', type=int, default=30, help='experiment time')
    parser.add_argument('output', help='directory')
    args = parser.parse_args()

    if not os.path.exists(args.output):
       os.makedirs(args.output)

    args.path   = "/home/danghu/build"

    pipes = []
    nodes = {'server' : 'node97', 'client' : 'node95' }

    pipes.append(server(nodes['server'], args.path, args.output))
    pipes.append(client(nodes['client'], args.path, nodes['server'], args.output))

    t= Timer(args.time, kill_all, pipes, nodes)
    t.start()

    for p in pipes:
        p.wait()

