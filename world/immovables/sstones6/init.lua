dirname = path.dirname(__file__)

world:new_immovable_type{
   name = "sstones6",
   descname = _"Standing Stones",
   size = "none",
   attributes = {},
   programs = {},
   animations = {
      idle = {
         pictures = { dirname .. "/idle.png" },
         player_color_masks = {},
         hotspot = { 7, 63 },
      },
   }
}
