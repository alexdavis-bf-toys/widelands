dirname = path.dirname(__file__)

world:new_immovable_type{
   name = "cactus1",
   descname = _"Cactus",
   size = "none",
   attributes = {},
   programs = {},
   animations = {
      idle = {
         pictures = { dirname .. "/idle.png" },
         player_color_masks = {},
         hotspot = { 10, 71 },
      },
   }
}

