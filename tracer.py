#!/usr/bin/env python3

from image import points

scale = 1
import turtle
turtle.speed(100000)
def sc(v):
    x, y = v
    return x * scale, y * scale

t = turtle.Turtle()
t.up()
for a, b, c in points:
    t.goto(*sc(a))
    t.down()
    t.goto(*sc(b))
    t.goto(*sc(c))
    t.goto(*sc(a))
    t.up()
input()
