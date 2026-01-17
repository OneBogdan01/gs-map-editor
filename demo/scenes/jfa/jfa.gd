@tool
extends Node2D

## Theoritically, we would need a few more backbuffer passes for a better distance field.
## Here, we made 6.
##
## Regarding how your project is contructed:
## I think the distant field is not supposed to be computed "on the fly" anyways.
## For provinces, that would be the case since their border don't change (only their owner).
## For countries, on the contrary, which layout could change, it could be computer with a GLSL shader in a background thread?
## It would be saved as an image regularily.

## ProvincesAASampled
## Just a test for a more performance AA sampling. Can be applied to a final Sprite2D referencing a viewport (country gradient, province borders, etc)
