docker build -f jenkins/Dockerfile.dev -t ioplacer .
docker run -v $(pwd):/ioplacer ioplacer bash -c "./ioplacer/jenkins/install.sh"