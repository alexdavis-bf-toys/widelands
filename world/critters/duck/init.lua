dirname = path.dirname(__file__)

animations = {
   idle = {
      pictures = path.list_directory(dirname, "duck_\\d+.png"),
      sound_effect = {
         directory = dirname,
         name = "duck",
      },
      hotspot = { 5, 7 },
      fps = 1,
   },
}
add_walking_animations(animations, dirname, "duck_walk", {5, 10})

world:new_critter_type{
   name = "duck",
   descname = _ "Duck",
   attributes = { "swimming" },
   programs = {
      remove = { "remove" },
   },
   animations = animations,
}
