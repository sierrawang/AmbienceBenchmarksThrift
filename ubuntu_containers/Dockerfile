# syntax=docker/dockerfile:1
# Tells docker which image to use
# The image is like a kernel image (but kinda cooler)
# and is the binary responsible for running your code
FROM ubuntu:18.04

# Then we add our dependencies with RUN
# Come back to this when things are not working because stuff
# might not be installed
# The syntax is RUN <bash command for stuff we want>
# try to keep this to one line
ADD script.sh /
RUN /bin/bash script.sh

# Specify the directory that the container will start in
WORKDIR /root/app

# Copy <src> <dst>
# This will copy the source directory on MY computer
# into the dest directory of the container
COPY . /root/app

# CMD tells the container to immediately run my application
# The given process is process 0 for the container
CMD ["ls"]

# Just because you expose a port on a container, doesnt mean it will
# be available. Docker will TRY to expose this port, but you also
# have to do port forwarding
# You have to specify this when you actually run the container
# You dont even have to run this in the docker file, its just for remembering
# here that you have to eventually do it
EXPOSE 3000
