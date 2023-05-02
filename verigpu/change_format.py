def format_number_to_string(number):
    #format number in 2's complement 32-bits binary and 32-bits hex, keep the leading 0s or 1s, separated by space
    number_out="{0:032b} {0:08x} {0}".format(number & 0xffffffff)
    print(number_out)

number=[
    2473,
    1,
    #xor
    2473^782,
    2473|1874
]
number=[
    15|3,
    15^3,
    3-15,
    -150>>3
]
#apply format_number_to_string to each number in the list number
list(map(format_number_to_string,number))





