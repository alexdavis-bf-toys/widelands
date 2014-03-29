dirname = path.dirname(__file__)

animations = {
   idle = {
      pictures = path.glob(dirname, "stag_idle_\\d+.png"),
      player_color_masks = {},
      hotspot = { 12, 26 },
      fps = 20,
   },
}
add_walking_animations(animations, dirname, "stag_walk", {25, 30}, 20)

world:new_critter_type{
   name = "stag",
   descname = _ "Stag",
   swimming = false,
   attributes = { "eatable" },
   programs = {
      remove = { "remove" },
   },
   animations = animations,
}
