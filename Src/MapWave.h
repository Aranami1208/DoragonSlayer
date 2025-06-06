#pragma once
#include "Displace.h"
#include "MapManager.h"


class MapWave : public MapBase {
public:
	MapWave();
	~MapWave();

	void MakeWaveMap(TextReader* txt, int n);

	void DrawDepth() override;
	void DrawScreen() override;

private:
	CWave* wave;
};