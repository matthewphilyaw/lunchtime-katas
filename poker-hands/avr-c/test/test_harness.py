import serial
import struct

card_face = {
    2 : '2',
    3 : '3',
    4 : '4',
    5 : '5',
    6 : '6',
    7 : '7',
    8 : '8',
    9 : '9',
    10 : 'T',
    11 : 'J',
    12 : 'Q',
    13 : 'K',
    14 : 'A'
}

card_suit = {
    0 : 'c',
    1 : 'd',
    2 : 'h',
    3 : 's'
}

hand_type = {
    0 : none,
    1 : high_card,
    2 : pair,
    3 : two_pair,
    4 : three_of_a_kind,
    5 : straight,
    6 : flush,
    7 : full_house,
    8 : four_of_a_kind,
    9 : straight_flush
}

rank_mask = ord(b'\x0f')
suit_mask = ord(b'\x30')
wild_mask = ord(b'\x80') # not used for now

hand = b'\x02\x23\x35\x14\x1e'

hands = {
    'high_card' : {
        'hand' : b'\x03\x15\x09\x2a\x1c',
        'result': b'\x2a\x1c\x09\x15\x03\x01'
    },
    'pair' : {
        'hand' : b'\x03\x13\x09\x2a\x1c',
        'result': b'\x03\x13\x2a\x1c\x09\x02'
    },
    'two_pair' : {
        'hand' : b'\x03\x13\x29\x39\x1c',
        'result': b'\x29\x39\x03\x13\x1c\x03'
    },
    'three_of_a_kind' : {
        'hand' : b'\x1c\x13\x29\x39\x19',
        'result': b'\x29\x39\x19\x1c\x13\x04'
    },
    'straight' : {
        'hand' : b'\x32\x05\x23\x34\x16',
        'result': b'\x16\x05\x34\x23\x32\x05'
    },
    'straight_ace_low' : {
        'hand' : b'\x32\x05\x23\x34\x1a',
        'result': b'\x05\x34\x23\x32\x1a\x05'
    },
    'flush' : {
        'hand' : b'\x23\x25\x29\x2a\x2c',
        'result': b'\x2a\x2c\x29\x25\x23\x26'
    },
    'full_house' : {
        'hand' : b'\x13\x03\x29\x39\x19',
        'result': b'\x29\x39\x19\x13\x03\x07'
    },
    'four_of_a_kind' : {
        'hand' : b'\x1c\x09\x29\x39\x19',
        'result': b'\x09\x29\x39\x19\x1c\x08'
    },
    'straight_flush' : {
        'hand' : b'\x32\x35\x33\x34\x36',
        'result': b'\x36\x35\x34\x33\x32\x09'
    },
    'straight_flush_ace_low' : {
        'hand' : b'\x32\x35\x33\x34\x3a',
        'result': b'\x35\x34\x33\x32\x3a\x09'
    }
}

def get_rank(c):
    ord(c) & rank_mask

def get_suit(c):
    (ord(c) & suit_mask) >> 4

def get_wild(c): # not used for now
    (ord(c) & wild_mask) >> 7

def check_for_ack(b):
    if ack != b'\xfe':
        raise Exception('Ranker 4000 did not send back ack - timed out')

def print_cards(output):
    for c in output:
        print('{0}{1}'.format(card_face[get_rank(c)], card_suit(get_suit(c))))

ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=1)

print('sent ready flag')
ser.write(b'\xff') # send ready flag

check_for_ack(serial.read(1)[0])
print('ranker 4000 responded with ack')

print('sending:')
print_cards(hands)
serial.write(hands)

check_for_ack(serial.read(1)[0])
print('ranker 4000 responded with ack')

output = serial.read(6)
print('received')
print_cards(output[:5])
print(hand_type[ord(output[-1])])

check_for_ack(serial.read(1)[0])
print('ranker 4000 responded with ack')