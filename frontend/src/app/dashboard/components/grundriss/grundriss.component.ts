import {Component, ElementRef, OnInit, ViewChild} from '@angular/core';
import * as d3 from 'd3';
import {Sensor} from "../../../shared/model/sensor";
import {RoomService} from "../../../shared/services/RoomService";
import {Room} from "../../common/room";

@Component({
  selector: 'app-grundriss',
  templateUrl: './grundriss.component.html',
  styleUrls: ['./grundriss.component.scss']
})
export class GrundrissComponent implements OnInit {
  @ViewChild('svgContainer', {static: true}) svgContainer!: ElementRef;
  public sensors: any[] = [
    {
      "id": "39d3d8d2-3b58-46dd-adc5-94cf54fec37f",
      "name": "CC093CA6A8ABF0A0_scd41_co2",
      "type": "co2",
      "createdAt": "2024-01-16T12:52:17.216401",
      "updatedAt": "2024-01-16T12:52:17.216418"
    },
    {
      "id": "e1b79e60-5725-414d-80b0-a162adf56df1",
      "name": "CC093CA6A8ABF0A0_scd41_hum",
      "type": "hum",
      "createdAt": "2024-01-16T12:52:17.760661",
      "updatedAt": "2024-01-16T12:52:17.760677"
    },
    {
      "id": "7d344014-35dc-42dc-bd2a-7a35cd3a45c3",
      "name": "CC093CA6A8ABF0A0_scd41_temp",
      "type": "temp",
      "createdAt": "2024-01-16T12:52:17.345736",
      "updatedAt": "2024-01-16T12:52:17.345753"
    },
    {
      "id": "60bec8fe-153f-41fb-9025-bff514f4ac46",
      "name": "CC093CA6A8ABF0A0_svm41_hum",
      "type": "hum",
      "createdAt": "2024-01-16T12:52:17.994166",
      "updatedAt": "2024-01-16T12:52:17.994182"
    },
    {
      "id": "33811c98-384b-4d2b-963e-cd941961bf8d",
      "name": "CC093CA6A8ABF0A0_svm41_nox",
      "type": "nox",
      "createdAt": "2024-01-16T12:52:18.627961",
      "updatedAt": "2024-01-16T12:52:18.627978"
    },
    {
      "id": "e3dd01e3-1b9f-4b4f-9091-412ee5c5ca08",
      "name": "CC093CA6A8ABF0A0_svm41_temp",
      "type": "temp",
      "createdAt": "2024-01-16T12:52:18.18901",
      "updatedAt": "2024-01-16T12:52:18.189027"
    },
    {
      "id": "74655ddf-a5cd-4b6a-87ed-4aad908daa9c",
      "name": "CC093CA6A8ABF0A0_svm41_voc",
      "type": "voc",
      "createdAt": "2024-01-16T12:52:18.394256",
      "updatedAt": "2024-01-16T12:52:18.394273"
    }
  ];
  public rooms: Room[] = [];

  constructor(private roomService: RoomService) {
  }

  ngOnInit(): void {
    this.roomService.getAllRooms().subscribe({
      next: (res: any) => {
        this.rooms = res.data;
        for (let i = 0; i < this.rooms.length; i++) {
          this.createSVG(this.rooms[i].sensors, i);
        }
      },
      error: (err: any) => console.log(err)
    });
  }

  createSVG(srcSensors: any[], id: number) {
    const width = 300;
    const height = 300;
    const radius = 130;
    const circleRadius = 15;
    console.log(srcSensors && srcSensors.length > 0);
    if (srcSensors && srcSensors.length > 0) {
      const div = d3.select('#svg-container').append('div')
        .attr('id', 'svg-container-' + id)
        .attr('class', 'col-12 col-sm-12 col-md-6 col-lg-4 col-xl-3 p-3 test-center');
      const svg = div.append('svg')
        .attr('width', width)
        .attr('height', height)
        .attr('class', 'border border-3 border-dark');

      let tooltip = div.append("div")
        .style("opacity", 0)
        .attr("class", "position-absolute rounded shadow bg-white border border-info border-1 p-2")
        .style("z-index", -1);

      let board = svg.append('circle')
        .attr('id', 'circle-board')
        .attr('class', 'cursor-pointer')
        .attr("cx", width / 2)
        .attr("cy", height / 2)
        .attr('r', 15)
        .attr('fill', 'gray');
      board.on('mousemove', () => {
        tooltip
          .html('NRF: ' + srcSensors[0].name.split('_')[0])
          .style("left", ((width / 2) + 70) + "px")
          .style("top", (height / 2) + "px")
          .style("z-index", 1000);
      }).on('mouseover', () => {
        tooltip.style("opacity", 1);
        board.style("stroke", "black").style("opacity", 1);
      }).on('mouseleave', () => {
        tooltip.style("opacity", 0).style("z-index", -1);
        board.style("stroke", "none").style("opacity", 1);
      });

      // Positionierung der Kreise am Umfang
      for (let i = 0; i < srcSensors.length; i++) {
        const angle = (i / srcSensors.length) * 2 * Math.PI;
        const cx = width / 2 + radius * Math.cos(angle);
        const cy = height / 2 + radius * Math.sin(angle);
        // Linien vom Kreis bis zum Zentrum
        svg.append('line')
          .attr('x1', cx)
          .attr('y1', cy)
          .attr('x2', width / 2)
          .attr('y2', height / 2)
          .attr('stroke', 'gray')
          .attr('stroke-width', 1);
        // Kreise am Umfang
        let circle = svg.append('circle')
          .attr('id', 'circle-' + i)
          .attr('class', 'cursor-pointer')
          .attr('cx', cx)
          .attr('cy', cy)
          .attr('r', circleRadius)
          .attr('fill', i % 2 === 0 ? 'blue' : 'red');

        circle.on('mousemove', () => {
          tooltip
            .html(srcSensors[i].name.split('_')[1] + '-' +
              srcSensors[i].name.split('_')[2] + '<br>value: ' + srcSensors[i].type)
            .style("left", (cx + 70) + "px")
            .style("top", (cy) + "px")
            .style("z-index", 1000);
        }).on('mouseover', () => {
          tooltip.style("opacity", 1);
          circle.style("stroke", "black").style("opacity", 1);
        }).on('mouseleave', () => {
          tooltip.style("opacity", 0).style("z-index", -1);
          circle.style("stroke", "none").style("opacity", 1);
        });
      }
    }

  }
}
