dirname = path.dirname(__file__)

animations = {
   idle = {
      pictures = { dirname .. "idle.png" },
      hotspot = { 42, 89 }
   },
}

world:new_immovable_type{
   name = "winterland_stones3",
   descname = _ "Stones 3",
   editor_category = "stones",
   size = "big",
   attributes = { "stone" },
   programs = {
      shrink = {
         "transform=winterland_stones2"
      }
   },
   animations = animations
}
