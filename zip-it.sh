#!/bin/bash

RELEASE=v1.1-beta1

(cd .. \
        && rm -f max-test-$RELEASE.zip \
        && zip -r max-test-$RELEASE.zip max-test -x 'max-test/source/maxsdk/*' 'max-test/.git/*')
