dirname = path.dirname(__file__)

world:new_immovable_type{
   name = "ruin4",
   descname = _ "Ruin",
   size = "none",
   attributes = {},
   programs = {},
   animations = {
      idle = {
         pictures = { dirname .. "/idle.png" },
         player_color_masks = {},
         hotspot = { 29, 36 },
      },
   }
}
