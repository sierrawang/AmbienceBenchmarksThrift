FROM fedora:36

ADD script.sh /
RUN /bin/bash script.sh

WORKDIR /root/app

COPY . /root/app

CMD ["ls"]

EXPOSE 3000
