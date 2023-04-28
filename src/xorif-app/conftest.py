#!/usr/bin/env python3

import pytest

def pytest_addoption(parser):
    parser.addoption(
        "--app_type", action="store", default=0, type=int, help="App type: 0 = C xorif-app, 1 = Python xorif-app"
    )
    parser.addoption(
        "--bf", action="store", default=0, type=int, help="Beamformer support: 0 = no, 1 = yes"
    )
    parser.addoption(
        "--ocp", action="store", default=0, type=int, help="O-RAN PXXCH Processor support: 0 = no, 1 = yes"
    )
    parser.addoption(
        "--oprach", action="store", default=0, type=int, help="O-RAN PRACH Processor support: 0 = no, 1 = yes"
    )

@pytest.fixture
def app_type(request):
    return request.config.getoption("--app_type")

@pytest.fixture
def bf(request):
    return request.config.getoption("--bf")

@pytest.fixture
def ocp(request):
    return request.config.getoption("--ocp")

@pytest.fixture
def oprach(request):
    return request.config.getoption("--oprach")