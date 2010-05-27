-- Basic tests
include "test_baseimmovables"

-- Player immovables
-- TODO: those should also work in the editor
if not wl.editor then
   -- Others
   include "test_flag"
   include "test_road"

   -- Buildings
   include "test_constructionsite"
   include "test_warehouse"
   include "test_productionsite"
   include "test_militarysite"
   include "test_trainingsite"
end