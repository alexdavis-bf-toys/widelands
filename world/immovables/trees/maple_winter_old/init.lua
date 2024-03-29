dirname = path.dirname(__file__)

world:new_immovable_type{
   name = "maple_winter_old",
   descname = _ "Maple (Old)",
   editor_category = "trees_deciduous",
   size = "small",
   attributes = { "tree" },
   programs = {
      program = {
         "animate=idle 1550000",
         "transform=deadtree4 39",
         "seed=maple_winter_sapling",
      },
      fall = {
         "remove=",
      },
   },
   animations = {
      idle = {
         pictures = path.list_directory(dirname, "idle_\\d+.png"),
         hotspot = { 23, 59 },
         fps = 10,
         sound_effect = {
            directory = "sound/animals",
            name = "bird4",
         },
      },
   },
}
