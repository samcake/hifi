//
//  PathBuilder.cpp
//  libraries/gpu/src/path
//
//  Created by Sam Gateau on 10/15/2015.
//  Copyright 2015 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//
#include "Path.h"

#include <sstream>

using namespace gpu;
using namespace pr;

int Path::Builder::evalNbCoords(Command command)
{
    switch (command)
    {
        case Command::CLOSE:
        case Command::CLOSE_ALIAS:
            return 0;
            break;
        case Command::MOVE_TO:
        case Command::RELATIVE_MOVE_TO:
            return 2;
            break;
        case Command::LINE_TO:
        case Command::RELATIVE_LINE_TO:
            return 2;
            break;
            
        case Command::HORIZONTAL_LINE_TO:
        case Command::RELATIVE_HORIZONTAL_LINE_TO:
        case Command::VERTICAL_LINE_TO:
        case Command::RELATIVE_VERTICAL_LINE_TO:
            return 1;
            break;
            
        case Command::CIRCULAR_CCW_ARC_TO:
        case Command::CIRCULAR_CW_ARC_TO:
            return 5;
            break;
        case Command::ARC_TO:
        case Command::RELATIVE_ARC_TO:
            return 7;
            break;
            
        case Command::QUADRATIC_CURVE_TO:
        case Command::RELATIVE_QUADRATIC_CURVE_TO:
            return 4;
            break;
        case Command::SMOOTH_QUADRATIC_CURVE_TO:
        case Command::RELATIVE_SMOOTH_QUADRATIC_CURVE_TO:
            return 2;
            break;
            
        case Command::CUBIC_CURVE_TO:
        case Command::RELATIVE_CUBIC_CURVE_TO:
            return 6;
            break;
        case Command::SMOOTH_CUBIC_CURVE_TO:
        case Command::RELATIVE_SMOOTH_CUBIC_CURVE_TO:
            return 4;
            break;
            
        default:
            return 0;
            break;
    }
}

int Path::Builder::evalJointTan(const Vec2& dir0, const Vec2& dir1, Vec2& bin0, Vec2& bin1, Vec2& joint0, Vec2& joint1) {
    float sqld0 = dir0.x*dir0.x + dir0.y*dir0.y;
    float sqld1 = dir1.x*dir1.x + dir1.y*dir1.y;
    
    if ((sqld0 != 0.f) && (sqld1 != 0.f)) {
        bin0 = Vec2(-dir0.y, dir0.x);
        bin1 =  Vec2(-dir1.y, dir1.x);
        
        joint0.x = dir0.x*dir1.x + dir0.y*dir1.y;
        joint0.y = bin0.x*dir1.x + bin0.y*dir1.y;
        
        joint1.x = dir1.x*dir0.x + dir1.y*dir0.y;
        joint1.y = bin1.x*dir0.x + bin1.y*dir0.y;
        
        return 2;
    } else if ((sqld0 == 0.f) && (sqld1 == 0.f)) {
        bin0 = bin1 = Vec2(0.f,0.f);
        
        joint0.x = -1.f;
        joint0.y = 0.f;
        joint1.x = -1.f;
        joint1.y = 0.f;
        return 0;
    } else if (sqld0 == 0.f) {
        bin0 = Vec2(0.f,0.f);
        bin1 = Vec2(-dir1.y, dir1.x);
        joint0.x = -1.f;
        joint0.y = 0.f;
        joint1.x = -1.f;
        joint1.y = 0.f;
        
        return 1;
    } else if (sqld1 == 0.f) {
        bin0 = Vec2(-dir0.y, dir0.x);
        bin1 = Vec2(0.f,0.f);
        joint0.x = -1.f;
        joint0.y = 0.f;
        joint1.x = -1.f;
        joint1.y = 0.f;
        return -1;
    }
    return 0;
}

int Path::Builder::applyJointCoords(Path::Segment::vector& segments, int prevEnd, int nextBegin, const Vec2& prevEndTan, const Vec2& nextBeginTan, Vec2& prevEndBin, Vec2 nextBeginBin) {
    Vec2 prevEndJoint, nextBeginJoint;
    int r = evalJointTan(prevEndTan, nextBeginTan, prevEndBin, nextBeginBin, prevEndJoint, nextBeginJoint);
    
    // prev command ending
    if (prevEnd >= 0) {
        int segIdx = prevEnd / 2;
        // r should be -1 or 2
        segments[ segIdx ].end.setBin(prevEndBin.x, prevEndBin.y);
        segments[ segIdx ].end.setJoint(prevEndJoint.x, prevEndJoint.y);
    }
    
    // current command begining
    if (nextBegin >= 0) {
        int segIdx = nextBegin / 2;
        // r should be 1 or 2
        segments[ segIdx ].start.setBin(nextBeginBin.x, nextBeginBin.y);
        segments[ segIdx ].start.setJoint(nextBeginJoint.x, nextBeginJoint.y);
    }
    
    return r;
}

int Path::Builder::finalizeVertex(Path::Segment::vector& segments,
                                  int& prevBegin, int& prevEnd, int nextBegin, int nextEnd,
                                  Vec2& prevBeginTan, Vec2& prevEndTan, Vec2& nextBeginTan, Vec2& nextEndTan,
                                  Vec2& prevEndBin, Vec2 nextBeginBin,
                                  const Path::Vertex& ncp, const Path::Vertex& npep,
                                  Path::Vertex& cp, Path::Vertex& pep) {
    int r = applyJointCoords(segments, prevEnd, nextBegin, prevEndTan, nextBeginTan, prevEndBin, nextBeginBin);
    
    // move on to the next command
    cp = ncp;
    pep = npep;
    
    prevBeginTan = nextBeginTan;
    prevEndTan = nextEndTan;
    prevBegin = nextBegin;
    prevEnd = nextEnd;
    
    return r;
}

int Path::Builder::finalizeJoint(Path::Segment::vector& segments, Point& prevEnd, Point& nextBegin) {
    return applyJointCoords(segments, prevEnd.idx, nextBegin.idx, prevEnd.tan, nextBegin.tan, prevEnd.bin, nextBegin.bin);
}

void Path::Builder::pushSegment(Path::Segment::vector& segments, Point& pb, Point& pe, float seglength) {
    pb.idx = int(segments.size()) * 2 ;
    pe.idx = pb.idx + 1;
    
    Path::Segment seg;
    
    seg.start.x = pb.pos.x;
    seg.start.y = pb.pos.y;
    seg.start.aL = pb.plen;
    seg.start.sL = pb.slen;
    seg.start.sl = seglength - pb.slen;
    
    seg.end.x = pe.pos.x;
    seg.end.y = pe.pos.y;
    seg.end.aL = pe.plen;
    seg.end.sL = pe.slen;
    seg.end.sl = seglength - pe.slen;
    
    // rest of the segment is included after
    
    segments.push_back(seg);
}

bool Path::Builder::closeSegment(Path::Segment::vector& segments) {
    if (   (start.idx != -1)
        &&  (prev.command != Command::CLOSE)
        &&  (prev.command != Command::MOVE_TO)
        &&  (prev.command != Command::RELATIVE_MOVE_TO)
        ) {
        if ((prev.end.pos.x == start.pos.x) && (prev.end.pos.y == start.pos.y)) {
            // Case 1:
            // Start = prev.end
            // just adjust the joint
            
            //  finalizeJoint((coords), prev.end, start);
            finalizeJoint((segments), prev.end, start);
            
            next.begin = prev.end;
            next.end.pos = start.pos;
            
            next.command = Command::CLOSE;
            
            // then move on start
            start.idx = next.end.idx;
            start.tan = Vec2(0.f, 0.f);
            
            next.end.idx = -1;
        } else {
            // Case 2:
            // Start != prev.end
            // Line To Start
            // And adjust joint
            
            next.end.pos = start.pos;
            next.begin.pos = prev.end.pos;
            next.command = Command::CLOSE;
            
            // segment
            Vec2 segment(next.end.pos.x - next.begin.pos.x, next.end.pos.y - next.begin.pos.y);
            
            // Length
            float segLength = length(segment);
            next.begin.plen = prev.end.plen;
            next.end.plen = next.begin.plen + segLength;
            
            next.begin.slen = 0.f;
            next.end.slen = segLength;
            
            // Tan is constant = cp to ncp
            next.begin.tan = normalize(segment);
            next.end.tan = next.begin.tan;
            
            // Draw line segment
            pushSegment((segments), next.begin, next.end, segLength);
            
            // Finalize the joint between prev and next
            int r = finalizeJoint((segments), prev.end, next.begin);
            
            // finlaize next end and begin and last
            finalizeJoint((segments), next.end, start);
            
            // then move on start
            start.idx = next.end.idx;
            start.tan = Vec2(0.f, 0.f);
            
            // invalidate the next.end to make sure it won't be overwritten ?
            next.end.idx = -1;
        }
    } else {
        // we skip the Z command but keep track of what was before for relative moves and drawings
    
        // move on to the next segment
        next.begin = prev.begin;
        next.end = prev.end;
        next.command = prev.command;
    }
    
    return true;
}


bool Path::Builder::addMoveTo(Path::Segment::vector& segments, Command command, const float* lcoords) {
    const float* c = lcoords;
    
    next.command = command;
    
    // grab ncp
    next.end.pos.x = c[0];
    next.end.pos.y = c[1];
    
    if (command == Command::RELATIVE_MOVE_TO)
    {
        next.end.pos.x = c[0] + prev.end.pos.x;
        next.end.pos.y = c[1] + prev.end.pos.y;
    }
    
    prev.end.pos = next.begin.pos;
    
    // reset abscissa
    next.begin.plen = next.end.plen = 0;
    next.begin.slen = next.end.slen = 0;
    
    // On moveto start change
    // it will be the next upcoming idx
    start.pos = next.end.pos;
    start.idx = int((segments).size()) * 2;
    
    // no next dir
    next.end.tan = next.begin.tan = Vec2(0.f, 0.f);
    
    // push the move to point as the current command begin
    next.end.idx = next.begin.idx = -1;
    
    // Finalize only if last command was usefull
    if (       (prev.command != Command::CLOSE)
        &&  (prev.command != Command::MOVE_TO)
        &&  (prev.command != Command::RELATIVE_MOVE_TO))
    {
        int r = finalizeJoint((segments), prev.end, next.begin);
    }
    
    return true;
}

bool Path::Builder::addLineTo(Path::Segment::vector& segments, Command command, const float* lcoords) {
    // grab ncp
    const float* c = lcoords;
    
    next.command = command;
    
    next.begin.pos = prev.end.pos;
    
    if (command == Command::LINE_TO)
    {
        next.end.pos.x = c[0];
        next.end.pos.y = c[1];
    }
    else if (command == Command::RELATIVE_LINE_TO)
    {
        next.end.pos.x = c[0] + next.begin.pos.x;
        next.end.pos.y = c[1] + next.begin.pos.y;
    }
    else if (command == Command::HORIZONTAL_LINE_TO)
    {
        next.end.pos.x = c[0];
        next.end.pos.y = next.begin.pos.y;
    }
    else if (command == Command::RELATIVE_HORIZONTAL_LINE_TO)
    {
        next.end.pos.x = c[0] + next.begin.pos.x;
        next.end.pos.y = next.begin.pos.y;
    }
    else if (command == Command::VERTICAL_LINE_TO)
    {
        next.end.pos.x = next.begin.pos.x;
        next.end.pos.y = c[0];
    }
    else if (command == Command::RELATIVE_VERTICAL_LINE_TO)
    {
        next.end.pos.x = next.begin.pos.x;
        next.end.pos.y = c[0] + next.begin.pos.y;
    }
    
    // segment
    Vec2 segment(next.end.pos.x - next.begin.pos.x, next.end.pos.y - next.begin.pos.y);
    
    // Length
    float segLength = length(segment);
    next.begin.plen = prev.end.plen;
    next.end.plen = next.begin.plen + segLength;
    
    next.begin.slen = 0.f;
    
    next.end.slen = segLength;
    
    // Tan is constant = cp to ncp
    next.begin.tan = normalize(segment);
    next.end.tan = next.begin.tan;
    
    // Draw line segment
    pushSegment(segments, next.begin, next.end, segLength);
    
    int r = finalizeJoint((segments), prev.end, next.begin);
    
    return true;
}

bool Path::Builder::addCircularArcTo(Path::Segment::vector& segments, Command command, const float* lcoords) {
    // grab ncp
    const float* c = lcoords;
    
    next.command = Command::CIRCULAR_CCW_ARC_TO;
    
    float CCW = ((command == Command::CIRCULAR_CCW_ARC_TO) ? 1.f : -1.f);
    
    // force angle in correct range
    float startAngle = fmod(c[3], 360.f);
    if (startAngle < 0.f)
        startAngle += 360.f;
    float endAngle = fmod(c[4], 360.f);
    if (endAngle < 0.f)
        endAngle += 360.f;
    
    float angleRange = 0.f;
    if (endAngle > startAngle) {
        angleRange = endAngle - startAngle;
        if (CCW < 0) {
            angleRange = angleRange - 360.f;
        }
    } else {
        angleRange = endAngle - startAngle;
        if (CCW > 0) {
            angleRange = 360.f + angleRange;
        }
    }
    
    // If prev ENd is not == to next start we should draw a line to keep continuity
    // if last segment is closed then restart from here
    
    // Eval arc circle start and end point A,B
    Vec2 angA(cos(startAngle*c_Deg2Rad), sin(startAngle*c_Deg2Rad));
    Vec2 A;
    A.x = c[0]+c[2]*angA.x;
    A.y = c[1]+c[2]*angA.y;
    Vec2 tA (-angA.y*CCW, +angA.x*CCW);
    
    Vec2 angB(cos(endAngle*c_Deg2Rad), sin(endAngle*c_Deg2Rad));
    Vec2 B;
    B.x = c[0]+c[2]*angB.x;
    B.y = c[1]+c[2]*angB.y;
    Vec2 tB (-angB.y*CCW, +angB.x*CCW);
    
    float radius = c[2];
    
    // cp should be start point
    //cp.x = A.x;
    //cp.y = A.y;
    
    next.begin.tan = tA;
    next.begin.pos = A;
    
    // Length
    float segLength = abs(angleRange*c_Deg2Rad) * c[2];
    next.begin.plen = prev.end.plen;
    next.end.plen = next.begin.plen + segLength ;
    
    next.begin.slen = 0.f;
    next.end.slen = segLength;
    
    next.end.tan = tB;
    next.end.pos = B;
    
    
    // Create points
    {
        //           int nbSubSegs = int(abs(angleRange) / 40.f);
        //          int nbSubSegs = int(abs(angleRange) / 10.f);
        int nbSubSegs = int(abs(angleRange) / 5.f);
        
        float invNbSubSegs = 1.f;
        if (nbSubSegs > 0)
            invNbSubSegs = 1.f / float(nbSubSegs);
        
        float angleDelta = angleRange * invNbSubSegs ;
        float lengthDelta = segLength * invNbSubSegs ;
        
        float cosHalfAngleDelta = cos(0.5*angleDelta*c_Deg2Rad);
        float sinHalfAngleDelta = sin(0.5*angleDelta*c_Deg2Rad);
        
        Point pA = next.begin;
        Point pB;
        
        for (int d = 1; d <= nbSubSegs; d++)
        {
            float angle = (startAngle + d * angleDelta)*c_Deg2Rad;;
            Vec2 angD(cos(angle), sin(angle));
            Vec2 D;
            D.x = c[0]+c[2]*angD.x;
            D.y = c[1]+c[2]*angD.y;
            Vec2 tD (-angD.y*CCW, +angD.x*CCW);
            
            Vec2 bD0, bD1, jD0, jD1;
            evalJointTan(tD, tD, bD0, bD1, jD0, jD1);
            
            float lengthInSeg = d * lengthDelta;
            float abscissa = next.begin.plen + d * abs(angleDelta*c_Deg2Rad) * c[2] ;
            
            if (d == nbSubSegs)
            {
                if (d == 1)
                    pushSegment(segments, next.begin, next.end, segLength);
                else
                    pushSegment(segments, pA, next.end, segLength);
            }
            else
            {
                pB.pos = D;
                pB.plen = abscissa;
                pB.slen = lengthInSeg;
                pB.bin = bD0;
                
                if (d == 1)
                    pushSegment(segments, next.begin, pB, segLength);
                else
                    pushSegment(segments, pA, pB, segLength);
            }
            
            Path::Segment* seg = &segments.back();
            
            seg->start.setBin(pA.bin.x, pA.bin.y);
            seg->start.setJoint(1.0, 0.0);
            seg->end.setBin(pB.bin.x, pB.bin.y);
            seg->end.setJoint(1.0, 0.0);
            
            seg->start.type = seg->end.type =  CCW * 1;
            seg->start.t0 = seg->end.t0 =  cosHalfAngleDelta;
            seg->start.t1 = -sinHalfAngleDelta;
            seg->end.t1 =  sinHalfAngleDelta;
            seg->start.t2 = seg->end.t2 =  radius;
            
            pA.pos = D;
            pA.plen = abscissa;
            pA.slen = lengthInSeg;
            pA.bin = bD1;
        }
    }
    
    if (start.idx == -1) {
        start.pos = next.begin.pos;
        start.tan = next.begin.tan;
        start.idx = next.begin.idx;
    }
    
    int r = finalizeJoint((segments), prev.end, next.begin);
    
    return true;
}


bool Path::Builder::addEllipticalArcTo(Path::Segment::vector& segments, Command command, const float* lcoords) {
    // grab ncp
    const float* c = lcoords;
    
    next.command = Command::ARC_TO;
    
    struct Arc
    {
        static float angle(float ux, float uy, float vx, float vy)
        {
            float t = ux*vy-uy*vx ;
            
            if (t >= 0.f)
            {
                return acos((ux * vx + uy * vy) / (sqrt(ux*ux + uy*uy) * sqrt(vx * vx + vy * vy)));
            }
            else
            {
                return -acos((ux * vx + uy * vy) / (sqrt(ux*ux + uy*uy) * sqrt(vx * vx + vy * vy)));
            }
        }
        
        
        float phi;
        float cphi;
        float sphi;
        float rx;
        float ry;
        float cx;
        float cy;
        float theta1;
        float dtheta;
        
        Arc(Vec2& p1, Vec2& p2, float pphi, float prx, float pry, bool fA, bool fS)
        {
            float x1 = p1.x;
            float y1 = p1.y;
            
            float x2 = p2.x;
            float y2 = p2.y;
            
            phi = fmod(pphi, 360.f);
            if (phi < 0.f)
                phi += 360.f;
            cphi = cos(phi * c_Deg2Rad);
            sphi = sin(phi * c_Deg2Rad);
            
            rx = prx;
            ry = pry;
            
            // Step 1:
            float x1p = cphi * (x1 - x2) * 0.5f + sphi * (y1 - y2) * 0.5f;
            float y1p = -sphi * (x1 - x2) * 0.5f + cphi * (y1 - y2) * 0.5f;
            
            float rpx = rx;
            float rpy = ry;
            float lambda = (x1p / rx)*(x1p / rx) + (y1p / ry)*(y1p / ry);
            if (lambda > 1.f)
            {
                float sqrtlambda = sqrt(lambda);
                rpx *= sqrtlambda;
                rpy *= sqrtlambda;
            }
            
            // Step 2:
            float fsgn = (fA != fS ? +1.f : -1.f);
            
            float rpx2 = rpx*rpx;
            float rpy2 = rpy*rpy;
            float x1p2 = x1p*x1p;
            float y1p2 = y1p*y1p;
            
            float cpx = fsgn * sqrt((rpx2*rpy2 - rpx2*y1p2 - rpy2*x1p2) / (rpx2 * y1p2 + rpy2 * x1p2)) * rpx * y1p / rpy;
            float cpy = fsgn * sqrt((rpx2*rpy2 - rpx2*y1p2 - rpy2*x1p2) / (rpx2 * y1p2 + rpy2 * x1p2)) * -rpy * x1p / rpx;
            
            // Step 3:
            cx = cphi * cpx - sphi*cpy + (x1 + x2) * 0.5f;
            cy = sphi * cpx + cphi*cpy + (y1 + y2) * 0.5f;
            
            // step 4:
            theta1 = Arc::angle(1, 0, (x1p - cpx)/rx, (y1p-cpy)/ry);
            
            float dangle = Arc::angle((x1p - cpx)/rx, (y1p-cpy)/ry, (-x1p - cpx)/rx, (-y1p-cpy)/ry);
            
            float d = dangle; // ?
            dtheta = 0.f;
            if (!fS)
            {
                if (d > 0.f)
                    dtheta = dangle - 2*c_PI;
                else
                    dtheta = dangle;
            }
            else
            {
                if (d < 0.f)
                    dtheta = dangle + 2*c_PI;
                else
                    dtheta = dangle;
            }
            
        }
        
        
        nv::Vec2 eval(float t)
        {
            float x = rx*cos(theta1+t*dtheta);
            float y = ry*sin(theta1+t*dtheta);
            
            float arc_x = cphi*x - sphi*y + cx;
            float arc_y = sphi*x + cphi*y + cy;
            
            return nv::Vec2(arc_x, arc_y);
        }
        
        nv::Vec2 evalTan(float t)
        {
            float tx = rx*(/*cos(theta1+t*dtheta) */-sin(theta1+t*dtheta));
            float ty = ry*(/*sin(theta1+t*dtheta)*/ cos(theta1+t*dtheta));
            
            float arcT_x = cphi*tx - sphi*ty;
            float arcT_y = sphi*tx + cphi*ty;
            
            return nv::Vec2(arcT_x, arcT_y);
        }
    };
    
    Arc arc(prev.end.pos, Vec2(c[5], c[6]), c[2], c[0], c[1], c[3] != 0, c[4] != 0);
    
    
    // If prev ENd is not == to next start we should draw a line to keep continuity
    // if last segment is closed then restart from here
    
    
    // cp should be start point
    //cp.x = A.x;
    //cp.y = A.y;
    
    next.begin.tan = normalize(arc.evalTan(0.0));
    next.begin.pos = arc.eval(0);
    
    // Length
    float segLength = abs(1*c_Deg2Rad) * c[1];
    next.begin.plen = prev.end.plen;
    next.end.plen = next.begin.plen + segLength ;
    
    next.begin.slen = 0.f;
    next.end.slen = segLength;
    
    next.end.tan = normalize(arc.evalTan(1.f));
    next.end.pos = arc.eval(1.f);
    
    
    // Create points
    {
        //           int nbSubSegs = int(abs(angleRange) / 40.f);
        //          int nbSubSegs = int(abs(angleRange) / 10.f);
        int nbSubSegs = NB_CURVE_SUB_SEGMENTS; //int(abs(angleRange) / 5.f);
        
        float invNbSubSegs = 1.f;
        if (nbSubSegs > 0)
            invNbSubSegs = 1.f / float(nbSubSegs);
        
        float angleDelta = 1 * invNbSubSegs ;
        float lengthDelta = 1 * invNbSubSegs ;
        
        Point pA;
        Point pB;
        
        for (int d = 1; d <= nbSubSegs; d++)
        {
            float angle = (0 + d * angleDelta);
            Vec2 D;
            D = arc.eval(angle);
            
            Vec2 tD (normalize(arc.evalTan(angle)));
            
            Vec2 bD0, bD1, jD0, jD1;
            evalJointTan(tD, tD, bD0, bD1, jD0, jD1);
            
            float lengthInSeg = d * lengthDelta;
            float abscissa = next.begin.plen + d * abs(angleDelta*c_Deg2Rad) ;
            
            if (d == nbSubSegs)
            {
                if (d == 1)
                    pushSegment(segments, next.begin, next.end, segLength);
                else
                    pushSegment(segments, pA, next.end, segLength);
            }
            else
            {
                pB.pos = D;
                pB.plen = abscissa;
                pB.slen = lengthInSeg;
                pB.bin = bD0;
                
                if (d == 1)
                    pushSegment(segments, next.begin, pB, segLength);
                else
                    pushSegment(segments, pA, pB, segLength);
            }
            
            Path::Segment* seg = &segments.back();
            
            seg->start.setBin(pA.bin.x, pA.bin.y);
            seg->start.setJoint(1.0, 0.0);
            seg->end.setBin(pB.bin.x, pB.bin.y);
            seg->end.setJoint(1.0, 0.0);
            
            /*  seg->start.type = seg->end.type =  CCW * 1;
             seg->start.t0 = seg->end.t0 =  cosHalfAngleDelta;
             seg->start.t1 = -sinHalfAngleDelta;
             seg->end.t1 =  sinHalfAngleDelta;
             seg->start.t2 = seg->end.t2 =  radius;
             */
            pA.pos = D;
            pA.plen = abscissa;
            pA.slen = lengthInSeg;
            pA.bin = bD1;
        }
    }
    
    if (start.idx == -1)
    {
        start.pos = next.begin.pos;
        start.tan = next.begin.tan;
        start.idx = next.begin.idx;
    }
    
    int r = finalizeJoint(segments, prev.end, next.begin);
    
    return true;
}

bool Path::Builder::addQuadraticCurveTo(Path::Segment::vector& segments, Command command, const float* lcoords) {
    // grab ncp
    const float* c = lcoords;
    
    struct Quadratic
    {
        Vec2 a, b, c;
        Quadratic()
        {
            a= b = c = Vec2(0.f);
        }
        Quadratic(Vec2 c0, Vec2 c1, Vec2 c2)
        {
            a= c0;
            b = c1;
            c  = c2;
        }
        
        
        nv::Vec2 eval(float t)
        {
            nv::Vec2 p;
            p.x = (1-t) * (1-t) * a.x + 2*(1-t)*t*b.x + t*t*c.x;
            p.y = (1-t) * (1-t) * a.y + 2*(1-t)*t*b.y + t*t*c.y;
            return p;
        }
        nv::Vec2 evalTan(float t)
        {
            nv::Vec2 p;
            p.x = (-2 + 2*t) * a.x + 2*(1-2*t)*b.x + 2*t*c.x;
            p.y = (-2 + 2*t) * a.y + 2*(1-2*t)*b.y + 2*t*c.y;
            return p;
        }
        
        void subdivide(float t, Quadratic* q0, Quadratic* q1)
        {
            Vec2 ab = a * (1-t) + b * t;
            Vec2 bc = b * (1-t) + c * t;
            Vec2 abc = ab * (1-t) + bc * t;
            
            q0->a = a;
            q0->b = ab;
            q0->c = q1->a = abc;
            q1->b = bc;
            q1->c = c;
        }
        
        float evalLength()
        {
            Vec2 la,lb;
            la.x = a.x - 2*b.x + c.x;
            la.y = a.y - 2*b.y + c.y;
            lb.x = 2*b.x - 2*a.x;
            lb.y = 2*b.y - 2*a.y;
            float A = 4*(la.x*la.x + la.y*la.y);
            float B = 4*(la.x*lb.x + la.y*lb.y);
            float C = lb.x*lb.x + lb.y*lb.y;
            
            float Sabc = 2*sqrt(A+B+C);
            float A_2 = sqrt(A);
            float A_32 = 2*A*A_2;
            float C_2 = 2*sqrt(C);
            float BA = B/A_2;
            
            return (A_32*Sabc + A_2*B*(Sabc-C_2) + (4*C*A-B*B)*log((2*A_2+BA+Sabc)/(BA+C_2)))/(4*A_32);
        }
        
        float evalLength(float t)
        {
            if (t <= 0) return 0.f;
            if (t >= 1.0) return evalLength();
            
            Quadratic q0, q1;
            subdivide(t, &q0, &q1);
            
            return q0.evalLength();
        };
        
        UINT evalTessellation(std::vector< Vec2 >& params, float error, float paramLength = 1.f)
        {
            UINT nbPoints = 0;
            
            float ab = length(b -a);
            float bc = length(c - b);
            float ac = length(c - a);
            float abc = ab+bc;
            if (abc > (1 + error) * ac)
            {
                Quadratic q0, q1;
                subdivide(0.5, &q0, &q1);
                nbPoints += q0.evalTessellation(params, error, paramLength * 0.5f);
                nbPoints += q1.evalTessellation(params, error, paramLength * 0.5f);
            }
            else
            {
                if (params.empty())
                {
                    params.push_back(Vec2(0.f,0.f));
                    nbPoints++;
                }
                Vec2 pl(paramLength + params.back().x, evalLength() + params.back().y);
                params.push_back(pl);
                nbPoints++;
            }
            return nbPoints;
        }
    };
    
    Vec2 controlpoint;
    
    // the new begin is the previ end.
    next.begin.pos = prev.end.pos;
    
    if (command == Command::QUADRATIC_CURVE_TO)
    {
        controlpoint.x = c[0];
        controlpoint.y = c[1];
        
        next.end.pos.x = c[2];
        next.end.pos.y = c[3];
    }
    else if (command == Command::RELATIVE_QUADRATIC_CURVE_TO)
    {
        controlpoint.x = next.begin.pos.x + c[0];
        controlpoint.y = next.begin.pos.y + c[1];
        
        next.end.pos.x = next.begin.pos.x + c[2];
        next.end.pos.y = next.begin.pos.y + c[3];
    }
    else if (command == Command::SMOOTH_QUADRATIC_CURVE_TO)
    {
        controlpoint = 2.f * next.begin.pos - prev.controlPoint();
        
        next.end.pos.x = c[0];
        next.end.pos.y = c[1];
    }
    else if (command == Command::RELATIVE_SMOOTH_QUADRATIC_CURVE_TO)
    {
        controlpoint = 2.f * next.begin.pos - prev.controlPoint();
        
        next.end.pos.x = next.begin.pos.x + c[0];
        next.end.pos.y = next.begin.pos.y + c[1];
    }
    
    Quadratic quad(next.begin.pos, controlpoint, next.end.pos);
    
    next.command = command;
    next.cp0 = controlpoint;
    
    
    // Let's generate the tesselation according to the error:
    std::vector< Vec2> params;
    UINT n = quad.evalTessellation(params, 0.001f);
    
    // Length
    float segLength = quad.evalLength();
    next.begin.plen = prev.end.plen;
    next.end.plen = next.begin.plen + segLength;
    
    next.begin.slen = 0.f;
    
    next.end.slen = segLength;
    
    // Tan is constant = cp to ncp
    next.begin.tan = normalize(quad.evalTan(0));
    next.end.tan = normalize(quad.evalTan(1));
    
    // Create points
    {
        Point pA;
        Point pB;
        
        int nbSubSegs = params.size() - 1;
        for (int d = 1; d <= nbSubSegs; d++)
        {
            float param = params[d].x;
            Vec2 D;
            D = quad.eval(param);
            
            Vec2 tD (normalize(quad.evalTan(param)));
            
            Vec2 bD0, bD1, jD0, jD1;
            evalJointTan(tD, tD, bD0, bD1, jD0, jD1);
            
            float lengthInSeg = params[d].y;
            float abscissa = next.begin.plen + lengthInSeg ;
            
            
            if (d == nbSubSegs)
            {
                if (d == 1)
                    pushSegment(segments, next.begin, next.end, segLength);
                else
                    pushSegment(segments, pA, next.end, segLength);
            }
            else
            {
                pB.pos = D;
                pB.plen = abscissa;
                pB.slen = lengthInSeg;
                pB.bin = bD0;
                
                if (d == 1)
                    pushSegment(segments, next.begin, pB, segLength);
                else
                    pushSegment(segments, pA, pB, segLength);
            }
            
            Path::Segment* seg = &segments.back();
            
            seg->start.setBin(pA.bin.x, pA.bin.y);
            seg->start.setJoint(1.0, 0.0);
            seg->end.setBin(pB.bin.x, pB.bin.y);
            seg->end.setJoint(1.0, 0.0);
            
            /*  seg->start.type = seg->end.type =  CCW * 1;
             seg->start.t0 = seg->end.t0 =  cosHalfAngleDelta;
             seg->start.t1 = -sinHalfAngleDelta;
             seg->end.t1 =  sinHalfAngleDelta;
             seg->start.t2 = seg->end.t2 =  radius;
             */
            pA.pos = D;
            pA.plen = abscissa;
            pA.slen = lengthInSeg;
            pA.bin = bD1;
        }
    }
    
    if (start.idx == -1)
    {
        start.pos = next.begin.pos;
        start.tan = next.begin.tan;
        start.idx = next.begin.idx;
    }
    
    int r = finalizeJoint((segments), prev.end, next.begin);
    
    return true;
}

bool Path::Builder::addCubicCurveTo(Path::Segment::vector& segments, Command command, const float* lcoords) {
    // grab ncp
    const float* c = lcoords;
    
    struct Cubic
    {
        Vec2 a, b, c, d;
        Cubic()
        {
            a= b = c = d = Vec2(0.f);
        }
        Cubic(Vec2& c0, Vec2& c1, Vec2& c2, Vec2& c3)
        {
            a= c0;
            b = c1;
            c  = c2;
            d  = c3;
        }
        
        nv::Vec2 eval(float t)
        {
            nv::Vec2 p;
            float t2 = t*t;
            float oneLessT = (1-t);
            float oneLessT2 = oneLessT * oneLessT;
            
            p.x = oneLessT2*oneLessT * a.x + 3*oneLessT2*t*b.x + 3*oneLessT*t2*c.x + t2*t*d.x;
            p.y = oneLessT2*oneLessT * a.y + 3*oneLessT2*t*b.y + 3*oneLessT*t2*c.y + t2*t*d.y;
            return p;
        }
        nv::Vec2 evalTan(float t)
        {
            nv::Vec2 p;
            float t2 = t*t;
            
            p.x = (-3+6*t-3*t2) * a.x + 3*(1-4*t+3*t2)*b.x + 3*(2*t-3*t2)*c.x + 3*t2*d.x;
            p.y = (-3+6*t-3*t2) * a.y + 3*(1-4*t+3*t2)*b.y + 3*(2*t-3*t2)*c.y + 3*t2*d.y;
            
            return p;
        }
        
        void subdivide(float t, Cubic* c0, Cubic* c1)
        {
            Vec2 ab = a * (1-t) + b * t;
            Vec2 bc = b * (1-t) + c * t;
            Vec2 cd = c * (1-t) + d * t;
            
            Vec2 abc = ab * (1-t) + bc * t;
            Vec2 bcd = bc * (1-t) + cd * t;
            
            Vec2 abcd = abc * (1-t) + bcd * t;
            
            c0->a = a;
            c0->b = ab;
            c0->c = abc;
            c0->d = c1->a = abcd;
            c1->b = bcd;
            c1->c = cd;
            c1->d = d;
        }
        
        float evalLength()
        {
            float ab = length(b -a);
            float bc = length(c - b);
            float cd = length(d - c);
            float ad = length(d - a);
            float abcd = ab+bc + cd;
            if (abcd > 1.01 * ad)
            {
                Cubic c0, c1;
                subdivide(0.5, &c0, &c1);
                return c0.evalLength() + c1.evalLength();
            }
            else
                return 0.5*(ad + abcd);
        }
        
        float evalLength(float t)
        {
            if (t <= 0) return 0.f;
            if (t >= 1.f) return evalLength();
            
            Cubic c0, c1;
            subdivide(t, &c0, &c1);
            return c0.evalLength();
        }
        
        UINT evalTessellation(std::vector< Vec2 >& params, float error, float paramLength = 1.f)
        {
            UINT nbPoints = 0;
            
            float ab = length(b -a);
            float bc = length(c - b);
            float cd = length(d - c);
            float ad = length(d - a);
            float abcd = ab+bc + cd;
            if (abcd > (1 + error) * ad)
            {
                Cubic c0, c1;
                subdivide(0.5, &c0, &c1);
                nbPoints += c0.evalTessellation(params, error, paramLength * 0.5f);
                nbPoints += c1.evalTessellation(params, error, paramLength * 0.5f);
            }
            else
            {
                if (params.empty())
                {
                    params.push_back(Vec2(0.f,0.f));
                    nbPoints++;
                }
                Vec2 pl(paramLength + params.back().x, (abcd + ad)*0.5f + params.back().y);
                params.push_back(pl);
                nbPoints++;
            }
            return nbPoints;
        }
    };
    
    Vec2 controlpoint0;
    Vec2 controlpoint1;
    
    // the new begin is the previ end.
    next.begin.pos = prev.end.pos;
    
    if (command == Command::CUBIC_CURVE_TO)
    {
        controlpoint0.x = c[0];
        controlpoint0.y = c[1];
        
        controlpoint1.x = c[2];
        controlpoint1.y = c[3];
        
        next.end.pos.x = c[4];
        next.end.pos.y = c[5];
    }
    else if (command == Command::RELATIVE_CUBIC_CURVE_TO)
    {
        controlpoint0.x = next.begin.pos.x + c[0];
        controlpoint0.y = next.begin.pos.y + c[1];
        
        controlpoint1.x = next.begin.pos.x + c[2];
        controlpoint1.y = next.begin.pos.y + c[3];
        
        next.end.pos.x = next.begin.pos.x + c[4];
        next.end.pos.y = next.begin.pos.y + c[5];
    }
    else if (command == Command::SMOOTH_CUBIC_CURVE_TO)
    {
        controlpoint0 = 2.f * next.begin.pos - prev.controlPoint();
        
        controlpoint1.x = c[0];
        controlpoint1.y = c[1];
        
        next.end.pos.x = c[2];
        next.end.pos.y = c[3];
    } else if (command == Command::RELATIVE_SMOOTH_CUBIC_CURVE_TO) {
        controlpoint0 = 2.f * next.begin.pos - prev.controlPoint();
        
        controlpoint1.x = next.begin.pos.x + c[0];
        controlpoint1.y = next.begin.pos.y + c[1];
        
        next.end.pos.x = next.begin.pos.x + c[2];
        next.end.pos.y = next.begin.pos.y + c[3];
    }
    
    Cubic cubic(next.begin.pos, controlpoint0, controlpoint1, next.end.pos);
    
    next.command = command;
    next.cp0 = controlpoint1;
    
    // Let's generate the tesselation according to the error:
    std::vector< Vec2> params;
    UINT n = cubic.evalTessellation(params, 0.001f);
    
    // Length
    float segLength = params.back().y;
    next.begin.plen = prev.end.plen;
    next.end.plen = next.begin.plen + segLength;
    
    next.begin.slen = 0.f;
    
    next.end.slen = segLength;
    
    // Tan is constant = cp to ncp
    next.begin.tan = normalize(cubic.evalTan(0));
    next.end.tan = normalize(cubic.evalTan(1));
    
    next.begin.pos = cubic.eval(0);
    next.end.pos = cubic.eval(1);
    
    // Create points
    {
        Point pA;
        Point pB;
        int nbSubSegs = params.size() - 1;
        for (int d = 1; d <= nbSubSegs; d++) {
            float param = params[d].x;
            Vec2 D;
            D = cubic.eval(param);
            
            Vec2 tD (normalize(cubic.evalTan(param)));
            
            Vec2 bD0, bD1, jD0, jD1;
            evalJointTan(tD, tD, bD0, bD1, jD0, jD1);
            
            float lengthInSeg =  params[d].y;
            float abscissa = next.begin.plen + lengthInSeg;
            
            if (d == nbSubSegs)
            {
                if (d == 1)
                    pushSegment(segments, next.begin, next.end, segLength);
                else
                    pushSegment(segments, pA, next.end, segLength);
            } else {
                pB.pos = D;
                pB.plen = abscissa;
                pB.slen = lengthInSeg;
                pB.bin = bD0;
                
                if (d == 1)
                    pushSegment(segments, next.begin, pB, segLength);
                else
                    pushSegment(segments, pA, pB, segLength);
            }
            
            Path::Segment* seg = &segments.back();
            
            seg->start.setBin(pA.bin.x, pA.bin.y);
            seg->start.setJoint(1.0, 0.0);
            seg->end.setBin(pB.bin.x, pB.bin.y);
            seg->end.setJoint(1.0, 0.0);
            
            /*  seg->start.type = seg->end.type =  CCW * 1;
             seg->start.t0 = seg->end.t0 =  cosHalfAngleDelta;
             seg->start.t1 = -sinHalfAngleDelta;
             seg->end.t1 =  sinHalfAngleDelta;
             seg->start.t2 = seg->end.t2 =  radius;
             */
            pA.pos = D;
            pA.plen = abscissa;
            pA.slen = lengthInSeg;
            pA.bin = bD1;
        }
    }
    
    int r = finalizeJoint((segments), prev.end, next.begin);
    
    return true;
}

uint32 Path::Builder::addSegment(std::vector< int >& paths, Path::Segment::vector& segments, Command command, const float* lcoords)
{
    uint32 nbSegmentsBefore = segments.size();
    switch (command)
    {
        case Command::CLOSE_ALIAS:
        case Command::CLOSE:
        {
            closeSegment(segments);
            
            // only odd and after the closing segment has been  eventually added
            if (paths.size() & 0x1)
                paths.push_back(start.idx);
        }
            break;
            
        case Command::MOVE_TO:
        case Command::RELATIVE_MOVE_TO:
        {
            // only odd, then end previous path
            if (paths.size() & 0x1)
                paths.push_back(prev.end.idx);
            
            addMoveTo(segments, command, lcoords);
        }
            break;
            
        case Command::LINE_TO:
        case Command::RELATIVE_LINE_TO:
        case Command::HORIZONTAL_LINE_TO:
        case Command::RELATIVE_HORIZONTAL_LINE_TO:
        case Command::VERTICAL_LINE_TO:
        case Command::RELATIVE_VERTICAL_LINE_TO:
        {
            // only even
            if (!(paths.size() & 0x1))
                paths.push_back(start.idx);
            addLineTo(segments, command, lcoords);
        }
            break;
            
        case Command::CIRCULAR_CCW_ARC_TO:
        case Command::CIRCULAR_CW_ARC_TO:
        {
            addCircularArcTo(segments, command, lcoords);
            // only even
            if (!(paths.size() & 0x1))
                paths.push_back(start.idx);
        }
            break;
        case Command::ARC_TO:
        case Command::RELATIVE_ARC_TO:
        {
            addEllipticalArcTo(segments, command, lcoords);
            // only even
            if (!(paths.size() & 0x1))
                paths.push_back(start.idx);
        }
            break;
        case Command::QUADRATIC_CURVE_TO:
        case Command::RELATIVE_QUADRATIC_CURVE_TO:
        case Command::SMOOTH_QUADRATIC_CURVE_TO:
        case Command::RELATIVE_SMOOTH_QUADRATIC_CURVE_TO:
        {
            addQuadraticCurveTo(segments, command, lcoords);
            // only even
            if (!(paths.size() & 0x1))
                paths.push_back(start.idx);
        }
            break;
            
        case Command::CUBIC_CURVE_TO:
        case Command::RELATIVE_CUBIC_CURVE_TO:
        case Command::SMOOTH_CUBIC_CURVE_TO:
        case Command::RELATIVE_SMOOTH_CUBIC_CURVE_TO:
        {
            addCubicCurveTo(segments, command, lcoords);
            // only even
            if (!(paths.size() & 0x1))
                paths.push_back(start.idx);
        }
            break;
            
        default:
            break;
    }
    
    // catch the startVertexBeginTan if needed
    if (next.begin.idx == start.idx)
    {
        start.tan = next.begin.tan;
    }
    
    // move on to the next segment
    prev.begin = next.begin;
    prev.end = next.end;
    prev.command = next.command;
    prev.cp0 = next.cp0;
    
    return segments.size() - nbSegmentsBefore;
}

uint32 Path::Builder::addSegmentAttribs(AttribVertices& attribs, const Format* attribFormat, uint32 nbSegmentsAdded, const Path::Segment::vector& coords, Command command, const float* lcoords) {
    uint32 offset = 0;
    switch (command)
    {
        case Command::CLOSE_ALIAS:
        case Command::CLOSE:
        {
            offset = attribFormat->nbAttribCoordsPerSegment();
            for (uint32 s = 0; s < nbSegmentsAdded; s++)
            {
                for (uint32 i = 0; i < offset; i++)
                    attribs.push_back(*(lcoords + i));
            }
        }
            break;
            
        case Command::MOVE_TO:
        case Command::RELATIVE_MOVE_TO:
        {
            offset = attribFormat->nbAttribCoordsPerMoveTo();
            for (uint32 s = 0; s < nbSegmentsAdded; s++)
            {
                for (uint32 i = 0; i < offset; i++)
                    attribs.push_back(*(lcoords + i));
            }
        }
            break;
            
        case Command::LINE_TO:
        case Command::RELATIVE_LINE_TO:
        case Command::HORIZONTAL_LINE_TO:
        case Command::RELATIVE_HORIZONTAL_LINE_TO:
        case Command::VERTICAL_LINE_TO:
        case Command::RELATIVE_VERTICAL_LINE_TO:
        case Command::CIRCULAR_CCW_ARC_TO:
        case Command::CIRCULAR_CW_ARC_TO:
        case Command::ARC_TO:
        case Command::RELATIVE_ARC_TO:
        case Command::QUADRATIC_CURVE_TO:
        case Command::RELATIVE_QUADRATIC_CURVE_TO:
        case Command::SMOOTH_QUADRATIC_CURVE_TO:
        case Command::RELATIVE_SMOOTH_QUADRATIC_CURVE_TO:
        case Command::CUBIC_CURVE_TO:
        case Command::RELATIVE_CUBIC_CURVE_TO:
        case Command::SMOOTH_CUBIC_CURVE_TO:
        case Command::RELATIVE_SMOOTH_CUBIC_CURVE_TO:
        {
            offset = attribFormat->nbAttribCoordsPerSegment();
            for (uint32 s = 0; s < nbSegmentsAdded; s++)
            {
                for (uint32 i = 0; i < offset; i++)
                    attribs.push_back(*(lcoords + i));
            }
        }
            break;
            
        default:
            break;
    }
    
    return offset;
}

int Path::Builder::generatePathBuffer(std::vector< int >& beginEnds, box2f& bound, CommandInfo::vector& commands, Path::Segment::vector& segments, Subpath::vector& subpaths, AttributeVertices& attribs,
                       const Data* pData, const Format* attribFormat)
{
    
    UINT totalNbSegmentsAdded = 0;
    UINT srcCoordOffset = 0;
    UINT nbCoords = 0;
    for (UINT i = 0; i < pData->NbCommands; i++)
    {
        nbCoords = evalNbCoords(pData->pSysMemCommands[i]);
        
        UINT nbSegmentsAdded = 0;
        if (((srcCoordOffset + nbCoords) <= pData->NbCoords))
        {
            CommandInfo com;
            com.beginSegmentOffset = segments.size();
            com.command = D3D11_PATH_COMMAND(pData->pSysMemCommands[i]);
            
            nbSegmentsAdded = addSegment(beginEnds, segments, D3D11_PATH_COMMAND(pData->pSysMemCommands[i]), pData->pSysMemCoords + srcCoordOffset);
            srcCoordOffset += nbCoords;
            
            com.nbSegments = nbSegmentsAdded;
            commands.push_back(com);
            
            totalNbSegmentsAdded += nbSegmentsAdded;
        }
        else
        {
            // Catastrophe !
            return 0;
        }
        
        // Try to grab Attribs if needed
        if (attribFormat && nbSegmentsAdded)
        {
            // And offset the SrcCOords accordingly
            srcCoordOffset += addSegmentAttribs(attribs, attribFormat, nbSegmentsAdded, segments, D3D11_PATH_COMMAND(pData->pSysMemCommands[i]), pData->pSysMemCoords + srcCoordOffset);
        }
        
    }
    
    // FInalize the last cap if last segment is not a close or move to
    if ((prev.command != Command::CLOSE) || (prev.command != Command::MOVE_TO) || (prev.command != Command::RELATIVE_MOVE_TO))
    {
        next.begin.tan.x = next.begin.tan.y = 0.f;
        next.begin.idx = -1;
        int r = applyJointCoords(segments, prev.end.idx, next.begin.idx, prev.end.tan, next.begin.tan, prev.end.bin, next.begin.bin);
        
        // on move change path so remember
        // if path not even means we need to close on previous
        if (beginEnds.size() & 0x1)
            beginEnds.push_back(prev.end.idx);
    }
    
    // if coords.empty() just return 0
    if (segments.empty())
        return false;
    
    // Eval the path length and the bounds
    subpaths.resize(int(beginEnds.size()) / 2);
    
    
    for (int p = 0; p < int(beginEnds.size()) / 2; p++)
    {
        int beginP = beginEnds[ 2 * p ] / 2;
        int endP = beginEnds[ 2 * p + 1 ] / 2;
        
        Subpath* subpath = &subpaths[ p ];
        subpath->beginSegmentOffset = beginP;
        subpath->nbSegments = (endP) - (beginP) + 1;
        subpath->beginPathOffset = p;
        subpath->bound.setNull();
        
        float pLength = segments[ endP ].end.aL;
        float pAbscissa = segments[ beginP ].start.aL;
        
        for (int s = (beginP); s <= (endP); s++)
        {
            segments[ s ].start.al = pLength - segments[ s  ].start.aL;
            
            bound += Vec2(segments[ s ].start.x, segments[ s ].start.y);
            subpath->bound += Vec2(segments[ s ].start.x, segments[ s ].start.y);
            
            segments[ s ].end.al = pLength - segments[ s ].end.aL;
            
            bound += Vec2(segments[ s ].end.x, segments[ s ].end.y);
            subpath->bound += Vec2(segments[ s ].end.x, segments[ s ].end.y);
            
            segments[ s ].beginOffset = s - (beginP);
            segments[ s ].endOffset = endP - s;
            segments[ s ].subPathIndex = p;
        }
    }
    
    return totalNbSegmentsAdded;
}

bool Path::Builder::buildPathBuffer(RenderData* path, const Desc* pDesc, const Data* pData);
{
    std::vector< int >  beginEnds;
    
    path->mBound.setNull();
    
    CommandInfo::vector& commands = path->mCommands;
    commands.clear();
    
    Path::Segment::vector& segments = path->mSegments;
    segments.clear();
    
    Subpath::vector& subpaths = path->mSubpaths;
    subpaths.clear();
    
    AttribVertex::vector& attribs = path->mAttribs;
    attribs.clear();
    
    const AttributeFormat* attribFormat =  path->mAttribsFormat;
    
    start.idx = -1;
    start.pos = Vec2(0.f);
    start.tan = Vec2(0.f);
    
    return generatePathBuffer(beginEnds, path->mBound, commands, segments, subpaths, attribs, pData, attribFormat);
}

// return the total number of segments added
int Path::Builder::rebuildPathBufferAddingCommands(RenderData* path, const Desc* pDesc, const Data* pAddedData)
{
    if (path->nbCommands() == 0)
        return buildPathBuffer(path, pDesc, pAddedData);
    
    std::vector< int >  beginEnds;
    
    CommandInfo::vector& commands = path->mCommands;
    
    Path::Segment::vector& segments = path->mSegments;
    
    Subpath::vector& subpaths = path->mSubpaths;
    
    AttribVertex::vector& attribs = path->mAttribs;
    
    const AttributeFormat* attribFormat =  path->mAttribsFormat;
    
    // preload the start / prev /next segments info
    
    UINT startCommandNb = path->mCommands.size();
    CommandInfo* commandBefore = 0;
    if (startCommandNb > 0)
    {
        commandBefore = &path->mCommands[startCommandNb - 1];
        if (  (commandBefore->command == Command::CLOSE)
            ||  (commandBefore->command == Command::CLOSE_ALIAS))
            commandBefore = 0;
    }
    
    int lastSegmentIdx = segments.size() - 1;
    Path::Segment* seg = &segments[lastSegmentIdx];
    Path::Segment* spStartSeg = &segments[lastSegmentIdx - seg->beginOffset];
    
    prev.end.pos = seg->end.pos();
    prev.end.tan = seg->end.tangent();
    prev.end.bin = seg->end.binormal();
    
    prev.end.plen = seg->end.aL;
    prev.end.slen = seg->end.sL;
    
    prev.cp0 = seg->start.pos();
    
    int beginEndsStartOffset = 0;
    if (seg->end.joint == -1)
    {
        prev.command = Command::CLOSE;
        start.idx = -1;
        start.pos = Vec2(0.f);
        start.tan = Vec2(0.f);
    }
    else
    {
        // if last command not closed then reassign the start correctly to the begin of the subpath
        prev.command = Command::LINE_TO;
        start.idx = (lastSegmentIdx - seg->beginOffset) * 2; // start idx is the vertex ID
        start.pos = spStartSeg->start.pos();
        start.tan = spStartSeg->start.tangent();
        
        // BeginEnds array is starting odd because the begin of the
        beginEnds.push_back(lastSegmentIdx - seg->beginOffset);
        
        beginEndsStartOffset = -1 ;
    }
    
    return generatePathBuffer(beginEnds, path->mBound, commands, segments, subpaths, attribs, pAddedData, attribFormat);
}


int Path::Builder::updateCommandCoords(RenderData* path, uint32 startCommandNb, uint32 nbCommands, uint32 nbCoords, const float* pSysMemCoords, uint32* touchedSegmentOffset) {
    if ((startCommandNb >= path-> nbCommands()) || ((startCommandNb < path-> nbCommands()) && (path-> nbCommands() - nbCommands <  startCommandNb)))
        return 0;
    Commands touchedCommands(nbCommands);
    UINT touchedNbCoords = 0;
    UINT touchedNbSegments = 0;
    CommandInfo* lcommandInfo = &path->mCommands[startCommandNb];
    CommandInfo* commandFirst = &path->mCommands[startCommandNb];
    for (int i = 0; i < nbCommands; i++)
    {
        touchedCommands[i] = lcommandInfo->command;
        touchedNbCoords += evalNbCoords(lcommandInfo->command);
        touchedNbSegments += lcommandInfo->nbSegments;
        lcommandInfo++;
    }
    CommandInfo* commandBefore = 0;
    if (startCommandNb > 0)
    {
        commandBefore = &path->mCommands[startCommandNb - 1];
        if (  (commandBefore->command == Command::CLOSE)
            ||  (commandBefore->command == Command::CLOSE_ALIAS))
            commandBefore = 0;
    }
    CommandInfo* commandAfter = 0;
    if (startCommandNb + nbCommands < path-> nbCommands())
    {
        commandAfter = &path->mCommands[startCommandNb + nbCommands];
    }
    
    // Check that we are good with the number of coords provided
    if (touchedNbCoords != nbCoords)
        return 0;
    
    // good to go, update
    D3D11_PATH_DATA newData = {0};
    newData.NbCoords = nbCoords;
    newData.pSysMemCoords = pSysMemCoords;
    newData.NbCommands = touchedCommands.size();
    newData.pSysMemCommands = touchedCommands.data();
    
    
    std::vector< int >  beginEnds;
    
    CommandInfo::vector commands;
    Path::Segment::vector segments;
    AttribVertex::vector  attribs;
    Subpath::vector     subpaths;
    box2f               newBound;
    
    if (!commandBefore)
    {
        prev.command = Command::CLOSE;
        start.idx = -1;
        start.pos = Vec2(0.f);
        start.tan = Vec2(0.f);
    }
    else if (  (commandBefore->command == Command::MOVE_TO)
             ||  (commandBefore->command == Command::RELATIVE_MOVE_TO))
    {
        prev.end.pos = prev.begin.pos = path->mSegments[ commandFirst->beginSegmentOffset ].start.pos();
        
        // reset abscissa
        prev.begin.plen = prev.end.plen = 0;
        prev.begin.slen = prev.end.slen = 0;
        
        // On moveto start change
        // it will be the next upcoming idx
        start.pos = prev.end.pos;
        start.idx = commandBefore->beginSegmentOffset * 2;
        
        // no next dir
        prev.end.tan = prev.begin.tan = Vec2(0.f, 0.f);
        
        // push the move to point as the current command begin
        prev.end.idx = prev.begin.idx = -1;
    }
    else
    {
        // if last command not closed then reassign the start correctly to the begin of the subpath
        prev.command = (D3D11_PATH_COMMAND) commandBefore->command;
        
        Path::Segment* seg = &path->mSegments[ commandFirst->beginSegmentOffset ];
        Path::Segment* prevseg = &path->mSegments[ commandFirst->beginSegmentOffset - 1 ];
        Path::Segment* spStartSeg = &path->mSegments[commandFirst->beginSegmentOffset - seg->beginOffset];
        
        
        start.idx = (commandFirst->beginSegmentOffset - seg->beginOffset) * 2; // start idx is the vertex ID
        start.pos = spStartSeg->start.pos();
        start.tan = spStartSeg->start.tangent();
        
        prev.end.pos = prevseg->end.pos();
        prev.end.tan = prevseg->end.tangent();
        prev.end.bin = prevseg->end.binormal();
        
        prev.end.plen = prevseg->end.aL;
        prev.end.slen = prevseg->end.sL;
        
        prev.cp0 = prevseg->start.pos();
        
        
        // BeginEnds array is starting odd because the begin of the
        beginEnds.push_back(commandFirst->beginSegmentOffset - seg->beginOffset);
    }
    
    // and generate the path data
    UINT newSegments = generatePathBuffer(beginEnds, newBound, commands, segments, subpaths, attribs, &newData, 0);
    
    // At this point we should have a new command and segmetns that looks like the old one
    // ready to update ?
    if (newSegments == touchedNbSegments)
    {
        
        
        memcpy(path->mSegments.data() + commandFirst->beginSegmentOffset, segments.data(), newSegments * sizeof(Path::Segment));
    }
    
    return touchedNbSegments;
}

