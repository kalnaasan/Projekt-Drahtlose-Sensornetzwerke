import {Element} from './element';
import {Room} from '../../dashboard/common/room';

export interface Shape {
  id: string;
  height: number;
  width: number;
  elements: Element[];
  rooms: Room[];
  createdAt: Date;
  updatedAt: Date;
}
