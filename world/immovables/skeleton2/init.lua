dirname = path.dirname(__file__)

world:new_immovable_type{
   name = "skeleton2",
   descname = _ "Skeleton",
   editor_category = "miscellaneous",
   size = "none",
   attributes = {},
   programs = {},
   animations = {
      idle = {
         pictures = { dirname .. "idle.png" },
         hotspot = { 26, 32 },
      },
   }
}
