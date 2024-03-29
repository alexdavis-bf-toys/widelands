dirname = path.dirname(__file__)

world:new_immovable_type{
   name = "maple_winter_sapling",
   descname = _ "Maple (Sapling)",
   editor_category = "trees_deciduous",
   size = "small",
   attributes = { "seed" },
   programs = {
      program = {
         "animate=idle 57500",
         "remove=21",
         "grow=maple_winter_pole",
      },
   },
   animations = {
      idle = {
         pictures = path.list_directory(dirname, "idle_\\d+.png"),
         hotspot = { 5, 12 },
         fps = 8,
      },
   },
}
