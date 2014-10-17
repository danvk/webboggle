#!/usr/bin/env python
'''Flask server which provides an API to webboggle.

Usage:

    GET /abcdefghijklmnop
    [
      ['fie', '11 02 01'],
      ['fin', '11 02 13'],
      ['fink', '11 02 13 22'],
      ['fino', '11 02 13 23'],
      ['glop', '21 32 23 33'],
      ['ink', '02 13 22'],
      ['jin', '12 02 13'],
      ['jink', '12 02 13 22'],
      ['knife', '22 13 02 11 01'],
      ...
    ]
'''

import re
import subprocess

COMMAND = ['./webboggle', 'scrabble-words.mmapped']

from flask import Flask, jsonify, Response

app = Flask(__name__)

@app.after_request
def after_request(response):
    # Live dangerously! Allow cross-domain XHRs for all requests.
    response.headers.add('Access-Control-Allow-Origin', '*')
    return response


@app.route("/<bd>")
def get_words(bd):
    bd = bd.lower()
    if not re.match(r'^[a-z.]*$', bd):
        return jsonify({
            'error': 'Invalid board "%s" (must match /[a-zA-Z.]*/)' % bd
        }), 400
    if not (len(bd) == 16 or (len(bd) == 17 and bd.count('qu') == 1)):
        return jsonify({
            'error': 'Invalid board "%s" (must have 16 letters, or 17 with a "qu"' % bd
        }), 400
    
    try:
        output = subprocess.check_output(COMMAND + [bd])
    except subprocess.CalledProcessError as e:
        return jsonify({
            'error': 'Boggle solver failed (%s)' % e
        }), 400

    words = [line.split(' ', 1) for line in set(output.split('\n')) if line]
    words.sort()
    return jsonify(words)


def run():
    app.run(host='0.0.0.0')


if __name__ == "__main__":
    app.run(host='0.0.0.0', debug=True)
