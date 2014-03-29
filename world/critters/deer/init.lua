dirname = path.dirname(__file__)

animations = {
   idle = {
      pictures = path.glob(dirname, "deer_idle_\\d+.png"),
      player_color_masks = {},
      hotspot = { 1, 10 },
      fps = 20,
   },
}
add_walking_animations(animations, dirname, "deer_walk", {15, 25}, 20)

world:new_critter_type{
   name = "deer",
   descname = _ "Deer",
   swimming = false,
   attributes = { "eatable" },
   programs = {
      remove = { "remove" },
   },
   animations = animations,
}
