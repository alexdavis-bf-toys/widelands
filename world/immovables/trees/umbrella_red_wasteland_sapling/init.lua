dirname = path.dirname(__file__)

world:new_immovable_type{
   name = "umbrella_red_wasteland_sapling",
   descname = _ "Red Umbrella Tree (Sapling)",
   category = "trees_wasteland",
   size = "small",
   attributes = { "seed" },
   programs = {
      program = {
         "animate=idle 38000",
         "remove=50",
         "grow=aspen_wasteland_pole",
      },
   },
   animations = {
      idle = {
         pictures = path.list_directory(dirname, "idle_\\d+.png"),
         player_color_masks = {},
         hotspot = { 5, 12 },
         fps = 8,
         sfx = {},
      },
   },
}
