dirname = path.dirname(__file__)

world:new_immovable_type{
   name = "skeleton3",
   descname = _ "Skeleton",
   -- category = "miscellaneous",
   size = "none",
   attributes = {},
   programs = {},
   animations = {
      idle = {
         pictures = { dirname .. "idle.png" },
         hotspot = { 47, 69 },
      },
   }
}
