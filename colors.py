def color(h, s, v):
    print('  CHSV(' + '{:.0f}'.format(h * 255 / 360)
        + ', ' + '{:.0f}'.format(s * 255 / 100)
        + ', ' + '{:.0f}'.format(v * 255 / 100) + '),')


def main():
    print(' lava\n')
    color(11, 76, 93)
    color(13, 72, 93)
    color(16, 69, 91)
    color(20, 67, 89)
    color(25, 75, 89)
    color(32, 75, 90)
    color(39, 70, 88)
    color(44, 72, 82)
    color(51, 69, 80)
    color(44, 72, 82)
    color(39, 70, 88)
    color(32, 75, 90)
    color(25, 75, 89)
    color(20, 67, 89)
    color(16, 69, 91)
    color(6, 72, 88)
    color(0, 75, 85)
    color(5, 77, 80)
    color(5, 83, 77)
    color(7, 79, 82)
    color(8, 77, 87)

    print('\n warnings\n')
    color(64, 95, 77)
    color(61, 87, 80)
    color(58, 90, 77)
    color(55, 93, 72)
    color(58, 90, 77)
    color(61, 87, 80)


if __name__ == '__main__':
    main()