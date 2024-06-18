run:
	@mkdir -p ./build
	@cd ./build;\
		cmake -DCMAKE_BUILD_TYPE=Debug ..;\
		make && ./VoxEngine_Entity_Creator

deploy:
	@mkdir -p ./build
	@cd ./build;\
		cmake -DCMAKE_BUILD_TYPE=Release ..;\
		make && ./VoxEngine_Entity_Creator

dmg:
	appdmg spec.json voxengine_entity_creator.dmg
