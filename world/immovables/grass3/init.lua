dirname = path.dirname(__file__)

world:new_immovable_type{
   name = "grass3",
   descname = _ "Grass",
   size = "none",
   attributes = {},
   programs = {},
   animations = {
      idle = {
         pictures = { dirname .. "/idle.png" },
         player_color_masks = {},
         hotspot = { 10, 11 },
      },
   }
}
