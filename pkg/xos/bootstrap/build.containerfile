FROM alpine:3.19
RUN apk add make xz

WORKDIR /root/xos
ADD pkg pkg
ENTRYPOINT /root/xos/pkg/xos/bootstrap/build.sh
