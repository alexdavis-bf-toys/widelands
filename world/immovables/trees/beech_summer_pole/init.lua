dirname = path.dirname(__file__)

world:new_immovable_type{
   name = "beech_summer_pole",
   descname = _ "Beech (Pole)",
   category = "trees_deciduous",
   size = "small",
   attributes = {},
   programs = {
      program = {
         "animate=idle 65000",
         "remove=24",
         "grow=beech_summer_mature",
      },
   },
   animations = {
      idle = {
         pictures = path.glob(dirname, "idle_\\d+.png"),
         player_color_masks = {},
         hotspot = { 12, 28 },
         fps = 8,
         sfx = {},
      },
   },
}
