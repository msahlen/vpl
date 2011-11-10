/* VPL - Vector path library
 * Copyright (C) 2009 - 2011Mattias Sahlén: <mattias.sahlen@gmail.com>
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General General Public License for more details.
 *
 * You should have received a copy of the GNU General General Public
 * License along with this library;
 * if not, see <http://www.gnu.org/licenses/>.
 */

#include "vpl.h"

int main()
{
	// Create a cube
	vpl::Path path;

	path.moveToAbs(100.0f,100.0f);
	path.lineToRel(100.0f,0.0f);
	path.lineToRel(0.0f,100.0f);
	path.lineToRel(-100.0f,0.0f);
	path.closePath();

	// Generate points
	vpl::PointGenerator pointGenerator;

	pointGenerator.generatePoints(path);

	float* points = pointGenerator.getPoints();
	vplUint numPoints = pointGenerator.getNumberOfPoints();

	STD_LOG_INFO() << "Dumping untransformed points";

	for(vplUint i = 0; i < numPoints; i++)
		STD_LOG_INFO() << *(points + 2*i) << "," << *(points + 2*i + 1);

	vpl::AffineMatrix matrix;

	matrix.translate(10.0f,10.0f);
	matrix.scale(2.0f);
	matrix.rotate(0.1f);

	matrix.transform(points,numPoints);

	STD_LOG_INFO() << "\nDumping transformed points";

	for(vplUint i = 0; i < numPoints; i++)
		STD_LOG_INFO() << *(points + 2*i) << "," << *(points + 2*i + 1);

	pointGenerator.generatePoints(path,matrix);
	
	points = pointGenerator.getPoints();
	numPoints = pointGenerator.getNumberOfPoints();
	
	STD_LOG_INFO() << "\nDumping transformed points(transformed during generation)";

	for(vplUint i = 0; i < numPoints; i++)
		STD_LOG_INFO() << *(points + 2*i) << "," << *(points + 2*i + 1);

	return 0;
}
