#include "ViewportsInitializer.h"

vector<D3D12_VIEWPORT> initializeViewports(Window* window) {
	vector<D3D12_VIEWPORT> viewports;

    D3D12_VIEWPORT fullPerspectiveVP;
    D3D12_VIEWPORT topLeftVP;
    D3D12_VIEWPORT topRightVP;
    D3D12_VIEWPORT bottomLeftVP;
    D3D12_VIEWPORT bottomRightVP;

    fullPerspectiveVP.TopLeftX = 0.0f;
    fullPerspectiveVP.TopLeftY = 0.0f;
    fullPerspectiveVP.Width = float(window->Width());
    fullPerspectiveVP.Height = float(window->Height());
    fullPerspectiveVP.MinDepth = 0.0f;
    fullPerspectiveVP.MaxDepth = 1.0f;

    topLeftVP.TopLeftX = 0.0f;
    topLeftVP.TopLeftY = 0.0f;
    topLeftVP.Width = float(window->Width() / 2);
    topLeftVP.Height = float(window->Height() / 2);
    topLeftVP.MinDepth = 0.0f;
    topLeftVP.MaxDepth = 1.0f;

    topRightVP.TopLeftX = float(window->Width() / 2);
    topRightVP.TopLeftY = 0.0f;
    topRightVP.Width = float(window->Width() / 2);
    topRightVP.Height = float(window->Height() / 2);
    topRightVP.MinDepth = 0.0f;
    topRightVP.MaxDepth = 1.0f;

    bottomLeftVP.TopLeftX = 0.0f;
    bottomLeftVP.TopLeftY = float(window->Height() / 2);
    bottomLeftVP.Width = float(window->Width() / 2);
    bottomLeftVP.Height = float(window->Height() / 2);
    bottomLeftVP.MinDepth = 0.0f;
    bottomLeftVP.MaxDepth = 1.0f;

    bottomRightVP.TopLeftX = float(window->Width() / 2);
    bottomRightVP.TopLeftY = float(window->Height() / 2);
    bottomRightVP.Width = float(window->Width() / 2);
    bottomRightVP.Height = float(window->Height() / 2);
    bottomRightVP.MinDepth = 0.0f;
    bottomRightVP.MaxDepth = 1.0f;    

    viewports.push_back(fullPerspectiveVP);
    viewports.push_back(topLeftVP);
    viewports.push_back(topRightVP);
    viewports.push_back(bottomLeftVP);
    viewports.push_back(bottomRightVP);

	return viewports;
}