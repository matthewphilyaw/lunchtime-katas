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
    0 : 'none',
    1 : 'high_card',
    2 : 'pair',
    3 : 'two_pair',
    4 : 'three_of_a_kind',
    5 : 'straight',
    6 : 'flush',
    7 : 'full_house',
    8 : 'four_of_a_kind',
    9 : 'straight_flush'
}

rank_mask = ord(b'\x0f')
suit_mask = ord(b'\x30')
wild_mask = ord(b'\x80') # not used for now

hands = [
    {
        'name' : 'high_card',
        'hand' : b'\x03\x15\x09\x2a\x1c',
        'expected': b'\x1c\x2a\x09\x15\x03\x01'
    },
    {
        'name' : 'pair',
        'hand' : b'\x03\x13\x09\x2a\x1c',
        'expected': b'\x03\x13\x1c\x2a\x09\x02'
    },
    {
        'name' : 'two_pair',
        'hand' : b'\x03\x13\x29\x39\x1c',
        'expected': b'\x29\x39\x03\x13\x1c\x03'
    },
    {
        'name' : 'three_of_a_kind',
        'hand' : b'\x1c\x13\x29\x39\x19',
        'expected': b'\x29\x39\x19\x1c\x13\x04'
    },
    {
        'name' : 'straight',
        'hand' : b'\x32\x05\x23\x34\x16',
        'expected': b'\x16\x05\x34\x23\x32\x05'
    },
    {
        'name' : 'straight_ace_low',
        'hand' : b'\x32\x05\x23\x34\x1e',
        'expected': b'\x05\x34\x23\x32\x1e\x05'
    },
    {
        'name' : 'flush',
        'hand' : b'\x23\x25\x29\x2a\x2c',
        'expected': b'\x2c\x2a\x29\x25\x23\x06'
    },
    {
        'name' : 'full_house',
        'hand' : b'\x13\x03\x29\x39\x19',
        'expected': b'\x29\x39\x19\x13\x03\x07'
    },
    {
        'name' : 'four_of_a_kind',
        'hand' : b'\x1c\x09\x29\x39\x19',
        'expected': b'\x09\x29\x39\x19\x1c\x08'
    },
    {
        'name' : 'straight_flush',
        'hand' : b'\x32\x35\x33\x34\x36',
        'expected': b'\x36\x35\x34\x33\x32\x09'
    },
    {
        'name' : 'straight_flush_ace_low',
        'hand' : b'\x32\x35\x33\x34\x3e',
        'expected': b'\x35\x34\x33\x32\x3e\x09'
    }
]

def get_rank(c):
    return c & rank_mask

def get_suit(c):
    return (c & suit_mask) >> 4

def get_wild(c): # not used for now
    return (c & wild_mask) >> 7

def check_for_ack(b):
    if b != b'\xfe':
        raise Exception('Ranker 4000 did not send back ack - timed out')

def print_cards(output):
    for c in output:
        print('{0}{1}'.format(card_face[get_rank(c)], card_suit[get_suit(c)]))


ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=3)

numtest = 0
numpass = 0
for test in hands:
    numtest += 1
    print('running ' + test['name'])
    print('sent ready flag\n')
    ser.write(b'\xff') # send ready flag

    check_for_ack(ser.read())
    print('ranker 4000 responded with ack\n')

    hand = test['hand']
    expected = test['expected']

    print('sending:')
    print_cards(hand)
    print()

    print('expected output:')
    print_cards(expected[:5])
    print(hand_type[expected[-1]])
    print()

    for c in hand:
        ba = bytearray(1)
        ba[0] = c
        ser.write(ba)
        check_for_ack(ser.read())

    check_for_ack(ser.read())
    print('ranker 4000 responded with ack\n')

    output = ser.read(6)

    print('received')
    print_cards(output[:5])
    print(hand_type[output[-1]])
    print()

    check_for_ack(ser.read())
    print('ranker 4000 responded with ack\n')

    print('verifying output')

    if expected == output:
        numpass += 1
        print('output matches expected output')
    else:
        print('output does not match expected output')

    print()
    print('-------------------------------------------')
    print()

if numpass == numtest:
    print('all tests passed')
else:
    print('failed test cases')
