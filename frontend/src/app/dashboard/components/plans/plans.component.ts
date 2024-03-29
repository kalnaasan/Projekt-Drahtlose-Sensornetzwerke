import {Component, OnInit} from '@angular/core';
import {ShapeService} from "../../../shared/services/shape.service";
import * as d3 from "d3";
import {Shape} from "../../../shared/model/shape";
import {RoomService} from "../../../shared/services/RoomService";
import {Router} from "@angular/router";

@Component({
  selector: 'app-grundriss',
  templateUrl: './plans.component.html',
  styleUrls: ['./plans.component.scss']
})
export class PlansComponent implements OnInit {
  private shape!: Shape;

  constructor(private shapeService: ShapeService,
              private roomService: RoomService,
              private router: Router) {
  }

  ngOnInit(): void {
    this.shapeService.getShapes().subscribe({
      next: (res: any) => {
        this.shape = res.data;
        console.log(this.shape);
        this.createShapes(res.data);
      },
      error: (err: any) => console.log(err)
    });
  }


  private convertDataToChartData(src: any) {
    for (let i = 0; i < src.valueMeasures.length; i++) {
      if (src.valueMeasures[i].type.toLowerCase().includes('temp') || src.valueMeasures[i].type.toLowerCase().includes('hum')) {
        src.valueMeasures[i].value = Math.round(src.valueMeasures[i].value / 1000);
      } /*else if (src.valueMeasures[i].type.toLowerCase().includes('voc')) {
        src.valueMeasures[i].value = Math.round(src.valueMeasures[i].value / 10);
      }*/
      src.valueMeasures[i].value = Math.round(src.valueMeasures[i].value);
    }
    return src;
  }

  createShapes(shapes: Shape[]) {
    for (let i = 0; i < shapes.length; i++) {
      this.createShape(shapes[i], i);
    }
  }

  editShape() {
    console.log('BTN EDIT')
  }

  onEditClick(id: string) {
    this.router.navigateByUrl('/plan/update/' + id).then(() => {
    });
  }

  createShape(shape: Shape, index: number) {
    const width: number = shape.width;
    const height: number = shape.height;

    const div = d3.select('#svg-container').append('div')
      .attr('id', 'svg-container-' + index)
      .attr('class', 'col-12 col-sm-12 col-md-6 col-lg-6 col-xl-6 p-3 position-relative');

    let btn = div.append("button")
      .attr('id', 'btn-' + index)
      .attr("class", "position-relative rounded-pill btn btn-primary me-3 d-flex align-items-center justify-content-center")
      .style('max-height', '35px')
      .on('click', () => {
        this.onEditClick(shape.id)
      })
      .append('mat-icon').html('edit')
      .attr('role', 'img')
      .attr('aria-hidden', 'true')
      .attr('class', 'mat-icon notranslate material-icons mat-ligature-font mat-icon-no-color');

    const svg = div.append('svg')
      .attr('width', width)
      .attr('height', height)
      .attr('class', 'border border-3 border-dark');
    let tooltip = div.append("div")
      .html('tooltip-' + index)
      .attr('id', 'tooltip-' + index)
      .style("opacity", 0)
      .attr("class", "position-absolute rounded-3 shadow-lg bg-white border border-info border-1 p-2")
      .style("z-index", -1);

    for (let i = 0; i < shape.elements.length; i++) {
      const startX = parseInt(shape.elements[i].start.split(':')[0], 10);
      const startY = shape.height - parseInt(shape.elements[i].start.split(':')[1], 10);
      const endX = parseInt(shape.elements[i].end.split(':')[0], 10);
      const endY = shape.height - parseInt(shape.elements[i].end.split(':')[1], 10);

      if (shape.elements[i].type.toLowerCase() === 'line') {
        svg.append('line')
          .attr('x1', startX)
          .attr('y1', startY)
          .attr('x2', endX)
          .attr('y2', endY)
          .attr('stroke', 'black')
          .attr('stroke-width', 3);
      } else if (shape.elements[i].type.toLowerCase() === 'sensor') {
        const circleRadius = 15;
        let circle = svg.append('circle')
          .attr('id', 'circle-' + i)
          .attr('class', 'cursor-pointer')
          .attr('cx', startX)
          .attr('cy', startY)
          .attr('r', circleRadius)
          .attr('fill', i % 2 === 0 ? 'blue' : 'red')
          .style('z-index', 999);

        circle.on('mousemove', () => {
          const id = shape.elements[i].room?.id || '';
          tooltip.style("left", startX + 70 + "px")
            .style("top", startY + "px")
            .style("z-index", 1000);
          this.roomService.getSensorValues(id).subscribe({
            next: (res: any) => {
              let content = shape.elements[i].room?.name + ':<br>';
              res.data = this.convertDataToChartData(res.data);
              for (const value of res.data.valueMeasures) {
                if (value.type.toLowerCase().includes('co2')) content += ' - CO2: ' + value.value + ' PPM' + '<br>';
                else if (value.type.toLowerCase().includes('hum')) content += ' - Humidity: ' + value.value + ' %' + '<br>';
                else if (value.type.toLowerCase().includes('temp')) content += ' - Temperature: ' + value.value + ' °C' + '<br>';
                else if (value.type.toLowerCase().includes('voc')) content += ' - VOC: ' + value.value + ' index' + '<br>';
              }
              tooltip.html(content).style("z-index", 1000);
            },
            error: (err: any) => console.log(err)
          });
        }).on('mouseover', () => {
          tooltip.style("opacity", 1);
          circle.style("stroke", "black").style("opacity", 1)
            .style('cursor', 'pointer');
        }).on('mouseleave', () => {
          tooltip.style("opacity", 0).style("z-index", -1);
          circle.style("stroke", "none").style("opacity", 1).style("z-index", 999);
        }).on('click', () => {
          console.log(shape.elements[i].room?.name);
          this.router.navigateByUrl('/rooms/' + shape.elements[i].room?.id).then(() => {
          });
        });
      }
    }
  }
}
