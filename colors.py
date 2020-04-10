def color(h, s, v):
    print('  CHSV(' + '{:.0f}'.format(h * 255 / 360)
        + ', ' + '{:.0f}'.format(s * 255 / 100)
        + ', ' + '{:.0f}'.format(v * 255 / 100) + '),')


def main():
    print(' lava\n')
    color(11, 96, 99)
    color(13, 92, 99)
    color(12, 89, 97)
    color(15, 87, 96)
    color(18, 95, 96)
    color(20, 95, 95)
    color(35, 90, 95)
    color(20, 92, 94)
    color(18, 89, 93)
    color(14, 92, 92)
    color(16, 90, 92)
    color(40, 95, 93)
    color(19, 95, 94)
    color(14, 87, 95)
    color(9, 89, 97)
    color(6, 92, 97)
    color(0, 95, 99)
    color(5, 97, 99)
    color(5, 99, 95)
    color(7, 99, 99)
    color(8, 97, 98)

    print('\n warnings\n')
    color(64, 95, 37)
    color(61, 97, 40)
    color(58, 92, 35)
    color(55, 93, 32)
    color(58, 90, 37)
    color(61, 97, 41)
    color(64, 95, 37)


if __name__ == '__main__':
    main()