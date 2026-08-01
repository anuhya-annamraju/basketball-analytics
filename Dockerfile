FROM --platform=linux/amd64 ubuntu:22.04 AS build
RUN apt-get update && apt-get install -y build-essential cmake ninja-build git ca-certificates
WORKDIR /app
COPY . .
RUN cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release && cmake --build build

FROM --platform=linux/amd64 ubuntu:22.04
WORKDIR /app
COPY --from=build /app/build/basketball-app /app/
COPY --from=build /app/snippets /app/snippets
ENTRYPOINT ["/app/basketball-app"]
CMD ["/app/snippets/snippet1.csv"]
