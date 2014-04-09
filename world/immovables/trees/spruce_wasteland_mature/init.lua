dirname = path.dirname(__file__)

world:new_immovable_type{
   name = "spruce_wasteland_mature",
   descname = _ "Spruce (Mature)",
   size = "small",
   attributes = {},
   programs = {
      program = {
         "animate=idle 60000",
         "remove=23",
         "grow=spruce_wasteland_old",
      },
   },
   animations = {
      idle = {
         pictures = path.glob(dirname, "idle_\\d+.png"),
         player_color_masks = {},
         hotspot = { 18, 48 },
         fps = 8,
         sfx = {},
      },
   },
}
