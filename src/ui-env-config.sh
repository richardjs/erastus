#!/usr/bin/env bash

# Placed in /docker-entrypoint.d and ran by nginx entrypoint
# See https://github.com/nginxinc/docker-nginx/blob/master/entrypoint/docker-entrypoint.sh

if [ -n "$MIN_ITERATIONS" ]; then echo MIN_ITERATIONS=$MIN_ITERATIONS\; >> js/config.js; fi
if [ -n "$MAX_ITERATIONS" ]; then echo MAX_ITERATIONS=$MAX_ITERATIONS\; >> js/config.js; fi
if [ -n "$MIN_WORKERS" ]; then echo MIN_WORKERS=$MIN_WORKERS\; >> js/config.js; fi
if [ -n "$MAX_WORKERS" ]; then echo MAX_WORKERS=$MAX_WORKERS\; >> js/config.js; fi
