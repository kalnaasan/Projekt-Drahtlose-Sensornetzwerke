import {Component, OnInit} from '@angular/core';
import {FormArray, FormBuilder, FormGroup, Validators} from '@angular/forms';
import {Shape} from '../../../shared/model/shape';
import * as d3 from 'd3';
import {RoomService} from '../../../shared/services/RoomService';
import {Element} from '../../../shared/model/element';
import {ShapeService} from '../../../shared/services/shape.service';
import {Router} from "@angular/router";

@Component({
  selector: 'app-plan-dialog',
  templateUrl: './plan.component.html',
  styleUrls: ['./plan.component.scss']
})
export class PlanComponent implements OnInit {

  public formGroup !: FormGroup;
  public shape!: Shape;
  protected svg: any;
  public rooms: any[] = [];

  constructor(private formBuilder: FormBuilder,
              private roomService: RoomService,
              private shapeService: ShapeService,
              private route: Router) {
    this.formGroup = this.formBuilder.group({
      width: ['', Validators.required],
      height: ['', Validators.required],
      elements: this.formBuilder.array([]),
    });
  }

  ngOnInit(): void {
    this.roomService.getAllRooms(true).subscribe({
      next: (res: any) => this.rooms = res.data,
      error: (err: any) => console.log(err)
    });
  }

  createShape() {
    const width = this.formGroup.get('width')?.value;
    const height = this.formGroup.get('height')?.value;
    this.svg = d3.select('#svg-container-dialog').append('svg')
      .attr('width', width)
      .attr('height', height)
      .attr('class', 'border border-dark border-2');
  }

  get elements() {
    return this.formGroup.controls['elements'] as FormArray;
  }

  get elementControls() {
    return (this.formGroup.controls['elements'] as FormArray).controls as FormGroup[];
  }

  addLine() {
    const elementFrom = this.formBuilder.group({
      type: ['Line', Validators.required],
      start: ['', Validators.required],
      end: ['', Validators.required]
    });
    this.elements.push(elementFrom);
  }

  addSensor() {
    const elementFrom = this.formBuilder.group({
      type: ['Sensor', Validators.required],
      start: ['', Validators.required],
      room: [{}, Validators.required]
    });
    this.elements.push(elementFrom);
  }

  addElementToSVG(index: number) {
    if (!this.svg) {
      this.createShape();
    }

    const element = this.formGroup.get('elements')?.value[index];
    if (element.type.toLowerCase() === 'line') {
      this.svg.append('line')
        .attr('id', 'line-' + index)
        .attr('x1', element.start.split(':')[0])
        .attr('y1', this.formGroup.get('height')?.value - element.start.split(':')[1])
        .attr('x2', element.end.split(':')[0])
        .attr('y2', this.formGroup.get('height')?.value - element.end.split(':')[1])
        .attr('stroke', 'gray')
        .attr('stroke-width', 3);
    } else if (element.type.toLowerCase() === 'sensor') {
      const circleRadius = 15;
      this.svg.append('circle')
        .attr('id', 'circle-' + index)
        .attr('class', 'cursor-pointer')
        .attr('cx', element.start.split(':')[0])
        .attr('cy', this.formGroup.get('height')?.value - element.start.split(':')[1])
        .attr('r', circleRadius)
        .attr('fill', index % 2 === 0 ? 'blue' : 'red');
    }

  }

  addShapeToFromGroup(): void {
    this.formGroup.get('width')?.setValue(this.shape.width);
    this.formGroup.get('height')?.setValue(this.shape.height);
    for (const item of this.shape.elements) {
      if (item.type.toLowerCase() === 'line') {
        const elementFrom = this.formBuilder.group({
          type: [item.type, Validators.required],
          start: [item.start, Validators.required],
          end: [item.end, Validators.required]
        });
        this.elements.push(elementFrom);
      } else if (item.type.toLowerCase() === 'sensor') {
        const elementFrom = this.formBuilder.group({
          type: [item.type, Validators.required],
          start: [item.start, Validators.required],
          room: [{}, Validators.required]
        });
        this.elements.push(elementFrom);
      }
    }
  }

  saveShape() {
    let newShape: Shape = {
      id: '',
      width: this.formGroup.get('width')?.value,
      height: this.formGroup.get('height')?.value,
      elements: [],
      rooms: [],
      createdAt: new Date(),
      updatedAt: new Date()
    };
    for (const element of this.elementControls) {
      let item: Element = {
        id: '',
        type: element.controls['type']?.value,
        start: element.controls['start']?.value,
        end: '00:00',
        shape: null,
        room: null,
        createdAt: new Date(),
        updatedAt: new Date()
      };
      if (element.controls['type'].value.toLowerCase() === 'line') {
        item.end = element.controls['end'].value;
      } else if (element.controls['type'].value.toLowerCase() === 'sensor') {
        item.room = element.controls['room'].value;
        newShape.rooms.push(element.controls['room'].value);
      }
      newShape.elements.push(item);
    }
    console.log(newShape);

    this.shapeService.createShape(newShape).subscribe({
      next: (res: any) => {
        console.log(res);
        this.route.navigateByUrl('/grundriss').then(() => {
        });
      },
      error: (err: any) => console.log(err)
    })
  }
}
