dirname = path.dirname(__file__)

world:new_immovable_type{
   name = "mushroom1",
   descname = _ "Mushroom",
   editor_category = "miscellaneous",
   size = "none",
   attributes = {},
   programs = {},
   animations = {
      idle = {
         pictures = { dirname .. "idle.png" },
         hotspot = { 6, 9 },
      },
   }
}
