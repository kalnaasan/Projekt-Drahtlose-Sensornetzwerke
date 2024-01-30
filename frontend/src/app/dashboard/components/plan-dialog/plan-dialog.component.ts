import {AfterViewInit, Component, OnInit} from '@angular/core';
import {FormArray, FormBuilder, FormGroup, Validators} from '@angular/forms';
import {Shape} from '../../../shared/model/shape';
import * as d3 from 'd3';
import {RoomService} from '../../../shared/services/RoomService';
import {Element} from '../../../shared/model/element';
import {ShapeService} from '../../../shared/services/shape.service';

@Component({
  selector: 'app-plan-dialog',
  templateUrl: './plan-dialog.component.html',
  styleUrls: ['./plan-dialog.component.scss']
})
export class PlanDialogComponent implements OnInit, AfterViewInit {

  public formGroup !: FormGroup;
  public shape!: Shape;
  protected svg: any;
  public rooms: any[] = [];

  constructor(private formBuilder: FormBuilder,
              private roomService: RoomService,
              private shapeService: ShapeService) {
    this.formGroup = this.formBuilder.group({
      width: ['', Validators.required],
      height: ['', Validators.required],
      elements: this.formBuilder.array([]),
    });
    this.shape = {
      id: '',
      width: 300,
      height: 300,
      elements: [
        {
          id: '',
          type: 'Line',
          start: '10:290',
          end: '290:290',
          shape: null,
          room: null,
          createdAt: new Date(),
          updatedAt: new Date()
        },
        {
          id: '',
          type: 'Line',
          start: '290:290',
          end: '290:160',
          shape: null,
          room: null,
          createdAt: new Date(),
          updatedAt: new Date()
        },
        {
          id: '',
          type: 'Line',
          start: '290:130',
          end: '290:10',
          shape: null,
          room: null,
          createdAt: new Date(),
          updatedAt: new Date()
        },
        {
          id: '',
          type: 'Line',
          start: '290:10',
          end: '10:10',
          shape: null,
          room: null,
          createdAt: new Date(),
          updatedAt: new Date()
        },
        {
          id: '',
          type: 'Line',
          start: '10:10',
          end: '10:290',
          shape: null,
          room: null,
          createdAt: new Date(),
          updatedAt: new Date()
        },
        {
          id: '',
          type: 'Line',
          start: '140:10',
          end: '140:130',
          shape: null,
          room: null,
          createdAt: new Date(),
          updatedAt: new Date()
        },
        {
          id: '',
          type: 'Line',
          start: '110:160',
          end: '110:290',
          shape: null,
          room: null,
          createdAt: new Date(),
          updatedAt: new Date()
        },
        {
          id: '',
          type: 'Line',
          start: '10:130',
          end: '110:130',
          shape: null,
          room: null,
          createdAt: new Date(),
          updatedAt: new Date()
        },
        {
          id: '',
          type: 'Line',
          start: '170:130',
          end: '290:130',
          shape: null,
          room: null,
          createdAt: new Date(),
          updatedAt: new Date()
        },
        {
          id: '',
          type: 'Line',
          start: '140:160',
          end: '290:160',
          shape: null,
          room: null,
          createdAt: new Date(),
          updatedAt: new Date()
        },
        {
          id: '',
          type: 'Sensor',
          start: '70:70',
          end: '0:',
          shape: null,
          room: null,
          createdAt: new Date(),
          updatedAt: new Date()
        },
        {
          id: '',
          type: 'Sensor',
          start: '220:70',
          end: '0:',
          shape: null,
          room: null,
          createdAt: new Date(),
          updatedAt: new Date()
        },
        {
          id: '',
          type: 'Sensor',
          start: '220:240',
          end: '0:',
          shape: null,
          room: null,
          createdAt: new Date(),
          updatedAt: new Date()
        },
        {
          id: '',
          type: 'Sensor',
          start: '60:210',
          end: '0:0',
          shape: null,
          room: null,
          createdAt: new Date(),
          updatedAt: new Date()
        }
      ],
      rooms: [],
      createdAt: new Date(),
      updatedAt: new Date(),
    }
    this.addShapeToFromGroup();
  }

  ngOnInit(): void {
    this.roomService.getAllRooms(true).subscribe({
      next: (res: any) => this.rooms = res.data,
      error: (err: any) => console.log(err)
    });
    this.createShape();
  }

  ngAfterViewInit(): void {
    this.createShape();
    for (let i = 0; i < this.elementControls.length; i++) {
      this.addElementToSVG(i);
    }
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
      next: (res: any) => console.log(res),
      error: (err: any) => console.log(err)
    })
  }
}
