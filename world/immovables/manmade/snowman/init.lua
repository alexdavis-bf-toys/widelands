dirname = path.dirname(__file__)

world:new_immovable_type{
   name = "snowman",
   descname = _ "Snowman",
   editor_category = "miscellaneous",
   size = "none",
   attributes = {},
   programs = {},
   animations = {
      idle = {
         pictures = { dirname .. "idle.png" },
         hotspot = { 9, 24 },
      },
   }
}
