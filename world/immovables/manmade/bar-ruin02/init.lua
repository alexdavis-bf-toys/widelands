dirname = path.dirname(__file__)

world:new_immovable_type{
   name = "bar-ruin02",
   descname = _ "Ruin",
   -- category = "miscellaneous",
   size = "medium",
   attributes = {},
   programs = {},
   animations = {
      idle = {
         pictures = { dirname .. "idle.png" },
         hotspot = { 43, 43 },
      },
   }
}
